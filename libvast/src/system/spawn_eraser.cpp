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

#include "vast/system/spawn_eraser.hpp"

#include "vast/concept/parseable/to.hpp"
#include "vast/concept/parseable/vast/expression.hpp"
#include "vast/concept/parseable/vast/time.hpp"
#include "vast/defaults.hpp"
#include "vast/error.hpp"
#include "vast/logger.hpp"
#include "vast/system/eraser.hpp"
#include "vast/system/node.hpp"
#include "vast/system/spawn_arguments.hpp"

#include <caf/settings.hpp>

namespace vast::system {

maybe_actor
spawn_eraser(system::node_actor* self, system::spawn_arguments& args) {
  using namespace std::string_literals;
  VAST_TRACE(VAST_ARG(self), VAST_ARG(args));
  // We require the ERASER to be started after INDEX and ARCHIVE components.
  VAST_ASSERT(self->state.index);
  VAST_ASSERT(self->state.archive);
  // Parse options.
  auto eraser_query = caf::get_or(args.inv.options, "system.aging-query", ""s);
  if (eraser_query.empty()) {
    VAST_VERBOSE(self, "has no aging-query and skips starting the eraser");
    return ec::no_error;
  }
  if (auto expr = to<expression>(eraser_query); !expr) {
    VAST_WARNING(self, "got an invalid aging-query", eraser_query);
    return expr.error();
  }
  auto aging_frequency = defaults::system::aging_frequency;
  if (auto str = caf::get_if<std::string>(&args.inv.options, "system.aging-"
                                                             "frequency")) {
    auto parsed = to<duration>(*str);
    if (!parsed)
      return parsed.error();
    aging_frequency = *parsed;
  }
  // Spawn the ERASER.
  auto res
    = self->spawn(eraser, aging_frequency, eraser_query, self->state.index,
                  caf::actor_cast<caf::actor>(self->state.archive));
  if (res)
    self->system().registry().put(atom::eraser_v, res);
  return res;
}

} // namespace vast::system
