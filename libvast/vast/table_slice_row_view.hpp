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
#include "vast/view.hpp"

namespace vast {

/// Convenience helper for traversing a row.
class table_slice_row_view {
public:
  table_slice_row_view(const table_slice& slice, size_t row) noexcept;

  /// @returns the data at given column.
  data_view operator[](size_t column) const;

  /// @returns the number of columns in the slice.
  size_t columns() const noexcept;

  /// @returns the viewed table slice.
  const table_slice& slice() const noexcept;

  /// @returns the viewed row.
  size_t row() const noexcept;

  template <class Inspector>
  friend auto inspect(Inspector& f, table_slice_row_view& x) {
    return f(caf::meta::type_name("table_slice_row_view"), x.slice_, x.row_);
  }

private:
  const table_slice& slice_;
  const size_t row_;
};

} // namespace vast
