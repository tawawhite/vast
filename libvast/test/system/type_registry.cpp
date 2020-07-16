// Copyright Tenzir GmbH. All rights reserved.

#define SUITE type_registry

#include "vast/system/type_registry.hpp"

#include "vast/test/fixtures/actor_system_and_events.hpp"
#include "vast/test/test.hpp"

#include "vast/defaults.hpp"
#include "vast/detail/notifying_stream_manager.hpp"
#include "vast/detail/spawn_container_source.hpp"
#include "vast/fwd.hpp"
#include "vast/system/exporter.hpp"
#include "vast/system/importer.hpp"
#include "vast/table_slice.hpp"
#include "vast/table_slice_builder_factory.hpp"
#include "vast/type.hpp"

#include <caf/stateful_actor.hpp>
#include <caf/test/dsl.hpp>

#include <stddef.h>

using namespace vast;

namespace {

const vast::record_type mock_layout_a = vast::record_type{
  {"a", vast::string_type{}},
  {"b", vast::count_type{}},
  {"c", vast::real_type{}},
}.name("mock");

vast::table_slice_ptr make_data_a(std::string a, vast::count b, vast::real c) {
  auto builder = factory<table_slice_builder>::make(
    defaults::import::table_slice_type, mock_layout_a);
  REQUIRE(builder);
  REQUIRE(builder->add(a, b, c));
  return builder->finish();
}

const vast::record_type mock_layout_b = vast::record_type{
  {"a", vast::string_type{}},
  {"b", vast::count_type{}},
  {"c", vast::real_type{}},
  {"d", vast::string_type{}},
}.name("mock");

vast::table_slice_ptr
make_data_b(std::string a, vast::count b, vast::real c, std::string d) {
  auto builder = factory<table_slice_builder>::make(
    defaults::import::table_slice_type, mock_layout_b);
  REQUIRE(builder);
  REQUIRE(builder->add(a, b, c, d));
  return builder->finish();
}

} // namespace

struct fixture : fixtures::deterministic_actor_system_and_events {
  fixture() {
    MESSAGE("spawning AUT");
    aut = spawn_aut();
    REQUIRE(aut);
    CHECK_EQUAL(state().data.size(), 0u);
  }

  ~fixture() {
    MESSAGE("shutting down AUT");
    self->send_exit(aut, caf::exit_reason::user_shutdown);
  }

  using type_registry_actor
    = system::type_registry_type::stateful_pointer<system::type_registry_state>;

  caf::actor spawn_aut() {
    auto handle = sys.spawn(system::type_registry, directory);
    sched.run();
    return caf::actor_cast<caf::actor>(handle);
  }

  system::type_registry_state& state() {
    return caf::actor_cast<type_registry_actor>(aut)->state;
  }

  caf::actor aut;
};

FIXTURE_SCOPE(type_registry_tests, fixture)

TEST(type_registry) {
  MESSAGE("importing mock data");
  {
    auto slices_a = std::vector{1000, make_data_a("1", 2u, 3.0)};
    auto slices_b = std::vector{1000, make_data_b("1", 2u, 3.0, "4")};
    vast::detail::spawn_container_source(sys, std::move(slices_a), aut);
    vast::detail::spawn_container_source(sys, std::move(slices_b), aut);
    run();
    CHECK_EQUAL(state().data.size(), 1u);
  }
  MESSAGE("retrieving layouts");
  {
    size_t size = -1;
    std::string name = "mock";
    self->send(aut, atom::get_v, name);
    run();
    bool done = false;
    self
      ->do_receive([&](vast::system::type_set result) {
        size = result.value.size();
        done = true;
      })
      .until(done);
    CHECK_EQUAL(size, 2u);
  }
  self->send_exit(aut, caf::exit_reason::user_shutdown);
}

FIXTURE_SCOPE_END()
