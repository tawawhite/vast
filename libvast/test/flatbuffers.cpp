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

#include "vast/chunk.hpp"
#include "vast/expression.hpp"
#include "vast/fbs/meta_index.hpp"
#include "vast/fbs/utils.hpp"
#include "vast/fbs/uuid.hpp"
#include "vast/meta_index.hpp"
#include "vast/msgpack_table_slice.hpp"
#include "vast/span.hpp"
#include "vast/system/index.hpp"
#include "vast/system/partition.hpp"
#include "vast/table_slice_header.hpp"
#include "vast/type.hpp"
#include "vast/uuid.hpp"

#define SUITE flatbuffers
#include "vast/test/test.hpp"

using vast::byte;
using vast::span;

TEST(uuid roundtrip) {
  vast::uuid uuid = vast::uuid::random();
  auto expected_fb = vast::fbs::wrap(uuid, vast::fbs::file_identifier);
  CHECK(expected_fb);
  auto fb = *expected_fb;
  vast::uuid uuid2 = vast::uuid::random();
  CHECK_NOT_EQUAL(uuid, uuid2);
  span<const byte> span{reinterpret_cast<const byte*>(fb->data()), fb->size()};
  vast::fbs::unwrap<vast::fbs::UUID>(span, uuid2);
  CHECK_EQUAL(uuid, uuid2);
}

TEST(meta index roundtrip) {
  // Prepare a mini meta index. The meta index only looks at the layout of the
  // table slices it gets, so we feed it with an empty table slice.
  auto meta_idx = vast::meta_index{};
  auto mock_partition = vast::uuid::random();
  vast::table_slice_header header;
  header.layout = vast::record_type{{"x", vast::count_type{}}}.name("y");
  auto slice = vast::msgpack_table_slice::make(header);
  REQUIRE(slice);
  meta_idx.add(mock_partition, *slice);
  // Serialize meta index.
  auto expected_fb = vast::fbs::wrap(meta_idx, vast::fbs::file_identifier);
  CHECK(expected_fb);
  auto fb = *expected_fb;
  span<const byte> span{reinterpret_cast<const byte*>(fb->data()), fb->size()};
  // Deserialize meta index.
  vast::meta_index recovered_meta_idx;
  vast::fbs::unwrap<vast::fbs::MetaIndex>(span, recovered_meta_idx);
  // Check that lookups still work as expected.
  auto candidates = recovered_meta_idx.lookup(vast::expression{
    vast::predicate{vast::key_extractor{".x"}, vast::equal, vast::data{0u}},
  });
  REQUIRE_EQUAL(candidates.size(), 1u);
  CHECK_EQUAL(candidates[0], mock_partition);
}

TEST(index roundtrip) {
  // TODO
  vast::system::v2::index_state state(/*self = */ nullptr);
}

TEST(partition roundtrip) {
  // TODO
  vast::system::v2::partition_state state;
}