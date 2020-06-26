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

#define SUITE schedule

#include "vast/schedule.hpp"

#include "vast/test/test.hpp"

#include <set>

TEST(basic operation) {
  vast::schedule schedule;
  CHECK_EQUAL(schedule.size(), 0u);
  // Generate two partition ids and two query ids.
  vast::uuid puuid1 = vast::uuid::random();
  vast::uuid puuid2 = vast::uuid::random();
  vast::uuid quuid1 = vast::uuid::random();
  vast::uuid quuid2 = vast::uuid::random();
  // A single query
  schedule.add(quuid1, puuid1);
  CHECK_EQUAL(schedule.size(), 1u);
  auto next = schedule.next();
  CHECK_EQUAL(next, puuid1);
  // One query using two partitions.
  CHECK_EQUAL(schedule.size(), 0u);
  schedule.add(quuid1, puuid1);
  schedule.add(quuid1, puuid2);
  CHECK_EQUAL(schedule.size(), 2u);
  auto next1 = schedule.next();
  auto next2 = schedule.next();
  CHECK_EQUAL((std::set<vast::uuid>{next1, next2}),
              (std::set<vast::uuid>{puuid1, puuid2}));
  // Two queries on the same partition.
  CHECK_EQUAL(schedule.size(), 0u);
  schedule.add(quuid1, puuid1);
  schedule.add(quuid2, puuid1);
  CHECK_EQUAL(schedule.size(), 1u);
  auto next_ = schedule.next();
  CHECK_EQUAL(next_, puuid1);
}