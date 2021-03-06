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

#include <type_traits>

#include "vast/concept/printable/core/printer.hpp"
#include "vast/concept/support/detail/variant.hpp"

namespace vast {

template <class Lhs, class Rhs>
class choice_printer;

template <class>
struct is_choice_printer : std::false_type {};

template <class Lhs, class Rhs>
struct is_choice_printer<choice_printer<Lhs, Rhs>> : std::true_type {};

template <class T>
constexpr bool is_choice_printer_v = is_choice_printer<T>::value;

/// Attempts to print either LHS or RHS.
template <class Lhs, class Rhs>
class choice_printer : public printer<choice_printer<Lhs, Rhs>> {
public:
  using lhs_attribute = typename Lhs::attribute;
  using rhs_attribute = typename Rhs::attribute;

  // LHS = unused && RHS = unused  =>  unused
  // LHS = T && RHS = unused       =>  LHS
  // LHS = unused && RHS = T       =>  RHS
  // LHS = T && RHS = T            =>  T
  // LHS = T && RHS = U            =>  variant<T, U>
  using attribute =
    std::conditional_t<
      std::is_same<lhs_attribute, unused_type>{}
        && std::is_same<rhs_attribute, unused_type>{},
      unused_type,
      std::conditional_t<
        std::is_same<lhs_attribute, unused_type>{},
        rhs_attribute,
        std::conditional_t<
          std::is_same<rhs_attribute, unused_type>{},
          lhs_attribute,
          std::conditional_t<
            std::is_same<lhs_attribute, rhs_attribute>{},
            lhs_attribute,
            detail::flattened_variant<lhs_attribute, rhs_attribute>
          >
        >
      >
    >;

  constexpr choice_printer(Lhs lhs, Rhs rhs)
    : lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {
  }

  template <class Iterator, class Attribute>
  bool print(Iterator& out, const Attribute& a) const {
    return print_left<Lhs>(out, a) || print_right(out, a);
  }

private:
  template <class Left, class Iterator, class Attribute>
  auto print_left(Iterator& out, const Attribute& a) const
  -> std::enable_if_t<is_choice_printer<Left>{}, bool> {
    return lhs_.print(out, a); // recurse
  }

  template <class Left, class Iterator>
  auto print_left(Iterator& out, unused_type) const
  -> std::enable_if_t<!is_choice_printer_v<Left>, bool> {
    return lhs_.print(out, unused);
  }

  template <class Left, class Iterator, class Attribute>
  auto print_left(Iterator& out, const Attribute& a) const
  -> std::enable_if_t<!is_choice_printer_v<Left>, bool> {
    auto x = caf::get_if<lhs_attribute>(&a);
    return x && lhs_.print(out, *x);
  }

  template <class Iterator>
  bool print_right(Iterator& out, unused_type) const {
    return rhs_.print(out, unused);
  }

  template <class Iterator, class Attribute>
  auto print_right(Iterator& out, const Attribute& a) const {
    auto x = caf::get_if<rhs_attribute>(&a);
    return x && rhs_.print(out, *x);
  }

  Lhs lhs_;
  Rhs rhs_;
};

} // namespace vast
