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

#include "vast/table_slice_row_view.hpp"

#include "vast/table_slice.hpp"

namespace vast {

table_slice_row_view::table_slice_row_view(const table_slice& slice,
                                           size_t row) noexcept
  : slice_{slice}, row_{row} {
  // nop
}

data_view table_slice_row_view::operator[](size_t column) const {
  VAST_ASSERT(column < columns());
  return slice_.at(row_, column);
}

size_t table_slice_row_view::columns() const noexcept {
  return slice_.columns();
}

/// @returns the viewed table slice.
const table_slice& table_slice_row_view::slice() const noexcept {
  return slice_;
}

/// @returns the viewed row.
size_t table_slice_row_view::row() const noexcept {
  return row_;
}

} // namespace vast
