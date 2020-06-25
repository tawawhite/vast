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

#include "vast/uuid.hpp"
#include "vast/chunk.hpp"
#include "vast/span.hpp"
#include "vast/system/partition.hpp"
#include "vast/fbs/utils.hpp"
#include "vast/meta_index.hpp"
#include "vast/fbs/utils.hpp"
#include "vast/fbs/uuid.hpp"
#include "vast/fbs/meta_index.hpp"

#define SUITE flatbuffers
#include "vast/test/test.hpp"

using namespace vast;

TEST(uuid roundtrip) {
  vast::uuid uuid = vast::uuid::random();
  auto expected_fb = fbs::wrap(uuid, vast::fbs::file_identifier);
  CHECK(expected_fb);
  auto fb = *expected_fb;
  vast::uuid uuid2 = vast::uuid::random();
  CHECK_NOT_EQUAL(uuid, uuid2);
  vast::span<const byte> span{reinterpret_cast<const byte*>(fb->data()), fb->size()};
  fbs::unwrap<fbs::UUID>(span, uuid2);
  CHECK_EQUAL(uuid, uuid2);
}

// TEST(partition roundtrip) {
//   vast::system::partition partition;
// }

TEST(meta index roundtrip) {
  auto meta_idx = vast::meta_index {};
  auto expected_fb = fbs::wrap(meta_idx, fbs::file_identifier);
  CHECK(expected_fb);
  auto fb = *expected_fb;
  vast::span<const byte> span{reinterpret_cast<const byte*>(fb->data()), fb->size()};
  vast::meta_index meta_idx_roundtrip;
  fbs::unwrap<fbs::MetaIndex>(span, meta_idx_roundtrip);
}