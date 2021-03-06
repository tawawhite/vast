/******************************************************************************
 *                    _   _____   __________                                  *
 *                   | | / / _ | / __/_  __/     Visibility                   *
 *                   | |/ / __ |_\ \  / /          Across                     *
 *                   |___/_/ |_/___/ /_/       Space and Time                 *
 *                                                                            *
 * This file is part of VAST. It is subject to the license terms in the       *
 * LICENSE file found in the top-level directory of this distribution and at  *
 * http://vast.io/license. No part of VAST, including this file, may be       *
 * copied, modified, propagated, or distributed except according to the terms *
 * contained in the LICENSE file.                                             *
 ******************************************************************************/

#pragma once

#include "vast/command.hpp"
#include "vast/concept/parseable/to.hpp"
#include "vast/concept/parseable/vast/endpoint.hpp"
#include "vast/concept/parseable/vast/expression.hpp"
#include "vast/concept/parseable/vast/schema.hpp"
#include "vast/defaults.hpp"
#include "vast/detail/make_io_stream.hpp"
#include "vast/detail/string.hpp"
#include "vast/endpoint.hpp"
#include "vast/error.hpp"
#include "vast/expression.hpp"
#include "vast/fwd.hpp"
#include "vast/logger.hpp"
#include "vast/schema.hpp"
#include "vast/system/accountant.hpp"
#include "vast/system/datagram_source.hpp"
#include "vast/system/signal_monitor.hpp"
#include "vast/system/source.hpp"
#include "vast/system/type_registry.hpp"

#include <caf/actor.hpp>
#include <caf/actor_cast.hpp>
#include <caf/actor_system.hpp>
#include <caf/io/middleman.hpp>
#include <caf/settings.hpp>
#include <caf/spawn_options.hpp>

namespace vast::system {

namespace {

caf::expected<expression> parse_expression(command::argument_iterator begin,
                                           command::argument_iterator end) {
  auto str = detail::join(begin, end, " ");
  auto expr = to<expression>(str);
  if (expr)
    expr = normalize_and_validate(*expr);
  return expr;
}

struct make_source_result {
  caf::actor src;
  std::string name;
};

} // namespace

/// Tries to spawn a new SOURCE for the specified format.
/// @tparam Reader the format-specific reader.
/// @tparam Defaults defaults for the format-specific reader.
/// @tparam SpawnOptions caf::spawn_options to pass to sys.spawn().
/// @param self Points to the parent actor.
/// @param sys The actor system to spawn the source in.
/// @param inv The invocation that prompted the actor to be spawned.
/// @param accountant A handle to the accountant component.
/// @param type_registry A handle to the type registry component.
/// @param importer A handle to the importer component.
/// @returns a handle to the spawned actor and the name of the reader on
///          success, an error otherwise.
template <class Reader, class Defaults,
          caf::spawn_options SpawnOptions = caf::spawn_options::no_flags,
          class Actor>
caf::expected<make_source_result>
make_source(const Actor& self, caf::actor_system& sys, const invocation& inv,
            accountant_type accountant, type_registry_type type_registry,
            caf::actor importer) {
  // Placeholder thingies.
  auto udp_port = std::optional<uint16_t>{};
  auto reader = std::unique_ptr<Reader>{nullptr};
  // Parse options.
  auto& options = inv.options;
  std::string category = Defaults::category;
  auto max_events = caf::get_if<size_t>(&options, "import.max-events");
  auto uri = caf::get_if<std::string>(&options, category + ".listen");
  auto file = caf::get_if<std::string>(&options, category + ".read");
  auto uds = get_or(options, category + ".uds", false);
  auto type = caf::get_if<std::string>(&options, category + ".type");
  auto slice_type = get_or(options, "import.table-slice-type",
                           defaults::import::table_slice_type);
  auto slice_size = get_or(options, "import.table-slice-size",
                           defaults::import::table_slice_size);
  if (slice_size == 0)
    return make_error(ec::invalid_configuration, "table-slice-size can't be 0");
  // Parse schema local to the import command.
  auto schema = get_schema(options, category);
  if (!schema)
    return schema.error();
  // Discern the input source (file, stream, or socket).
  if (uri && file)
    return make_error(ec::invalid_configuration, "only one source possible (-r "
                                                 "or -l)");
  if (!uri && !file) {
    using inputs = vast::format::reader::inputs;
    if constexpr (Reader::defaults::input == inputs::inet)
      uri = std::string{Reader::defaults::uri};
    else
      file = std::string{Reader::defaults::path};
  }
  if (uri) {
    endpoint ep;
    if (!vast::parsers::endpoint(*uri, ep))
      return make_error(vast::ec::parse_error, "unable to parse endpoint",
                        *uri);
    if (ep.port.type() == port::unknown) {
      using inputs = vast::format::reader::inputs;
      if constexpr (Reader::defaults::input == inputs::inet) {
        endpoint default_ep;
        vast::parsers::endpoint(Reader::defaults::uri, default_ep);
        ep.port = port{ep.port.number(), default_ep.port.type()};
      } else {
        // Fall back to tcp if we don't know anything else.
        ep.port = port{ep.port.number(), port::tcp};
      }
    }
    reader = std::make_unique<Reader>(slice_type, options);
    VAST_INFO_ANON(reader->name(), "listens for data on",
                   ep.host + ":" + to_string(ep.port));
    switch (ep.port.type()) {
      default:
        return make_error(vast::ec::unimplemented,
                          "port type not supported:", ep.port.type());
      case port::udp:
        udp_port = ep.port.number();
        break;
    }
  } else {
    auto in = detail::make_input_stream(*file, uds);
    if (!in)
      return in.error();
    reader = std::make_unique<Reader>(slice_type, options, std::move(*in));
    if (*file == "-")
      VAST_INFO_ANON(reader->name(), "reads data from stdin");
    else
      VAST_INFO_ANON(reader->name(), "reads data from", *file);
  }
  if (!reader)
    return make_error(ec::invalid_result, "failed to spawn reader");
  VAST_VERBOSE_ANON(reader->name(), "produces", slice_type, "table slices of",
                    slice_size, "events");
  // Spawn the source, falling back to the default spawn function.
  auto local_schema = schema ? std::move(*schema) : vast::schema{};
  auto type_filter = type ? std::move(*type) : std::string{};
  auto src =
    [&](auto&&... args) {
      if (udp_port)
        return sys.middleman().spawn_broker<SpawnOptions>(
          datagram_source<Reader>, *udp_port,
          std::forward<decltype(args)>(args)...);
      else
        return sys.spawn<SpawnOptions>(source<Reader>,
                                       std::forward<decltype(args)>(args)...);
    }(std::move(*reader), slice_size, max_events, std::move(type_registry),
      std::move(local_schema), std::move(type_filter), std::move(accountant));
  VAST_ASSERT(src);
  // Attempt to parse the remainder as an expression.
  if (!inv.arguments.empty()) {
    auto expr = parse_expression(inv.arguments.begin(), inv.arguments.end());
    if (!expr)
      return expr.error();
    self->send(src, std::move(*expr));
  }
  // Connect source to importer.
  if (!importer)
    return make_error(ec::missing_component, "importer");
  VAST_DEBUG(inv.full_name, "connects to", VAST_ARG(importer));
  self->send(src, atom::sink_v, importer);
  return make_source_result{src, reader->name()};
}

} // namespace vast::system
