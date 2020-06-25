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

#include "vast/uuid.hpp"

#include <deque>

namespace vast {

/// Determines the order in which partitions are activated by the INDEX.
class schedule {
public:
  /// Number of scheduled partitions.
  size_t size();

  /// Return the partition that should be scheduled next, and remove it
  /// from the internal queue. Undefined behaviour if called when empty.
  uuid next();

  /// Inform the schedule that the given query depends on the given partition.
  void add(const uuid& query_id, const uuid& partition_id);

private:
  // TODO: Currently this uses a very simple FIFO queue. Eventually, this
  // will need to become more advanced to consider both the number of queries
  // waiting for a partition as well as some upper time bound to prevent
  // starvation.
  std::deque<uuid> queue_;
};

} // namespace vast