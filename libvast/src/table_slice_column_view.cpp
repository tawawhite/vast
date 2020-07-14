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

#include "vast/table_slice_column_view.hpp"

#include "vast/table_slice.hpp"

namespace vast {

table_slice_column_view::table_slice_column_view(const table_slice& slice,
                                                 size_t column) noexcept
  : slice_{slice}, column_{column} {
  // nop
}

data_view table_slice_column_view::operator[](size_t row) const {
  VAST_ASSERT(row < rows());
  return slice_.at(row, column_);
}

size_t table_slice_column_view::rows() const noexcept {
  return slice_.rows();
}

const table_slice& table_slice_column_view::slice() const noexcept {
  return slice_;
}

size_t table_slice_column_view::column() const noexcept {
  return column_;
}

} // namespace vast
