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

#include "vast/fwd.hpp"
#include "vast/type.hpp"

#include <caf/meta/type_name.hpp>

namespace vast {

/// The header of a table slice.
/// @relates table_slice
struct table_slice_header {
  record_type layout = {}; ///< The flattened layout of the data.
  uint64_t rows = 0;       ///< The number of events (= rows).
  id offset = 0;           ///< The offset in the 2^64 ID event space.

  /// Type inspection support for CAF.
  template <class Inspector>
  friend auto inspect(Inspector& f, table_slice_header& x) {
    return f(caf::meta::type_name("table_slice_header"), x.layout, x.rows,
             x.offset);
  }
};

} // namespace vast
