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

/// Convenience helper for traversing a column.
class table_slice_column_view {
public:
  table_slice_column_view(const table_slice& slice, size_t column) noexcept;

  /// @returns the data at given row.
  data_view operator[](size_t row) const;

  /// @returns the number of rows in the slice.
  size_t rows() const noexcept;

  /// @returns the viewed table slice.
  const table_slice& slice() const noexcept;

  /// @returns the viewed column.
  size_t column() const noexcept;

  template <class Inspector>
  friend auto inspect(Inspector& f, table_slice_column_view& x) {
    return f(caf::meta::type_name("table_slice_column_view"), x.slice_,
             x.column_);
  }

private:
  const table_slice& slice_;
  const size_t column_;
};

} // namespace vast
