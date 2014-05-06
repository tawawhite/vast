#ifndef VAST_REGEX_H
#define VAST_REGEX_H

#include "vast/config.h"

#include <regex>
#include "vast/parse.h"
#include "vast/string.h"

namespace vast {

/// A regular expression.
class regex : util::totally_ordered<regex>
{
public:
  /// Constructs a regex from a glob expression. A glob expression consists
  /// of the following elements:
  ///
  ///     - `*`   Equivalent to `.*` in a regex
  ///
  ///     - `?`    Equivalent to `.` in a regex
  ///
  ///     - `[ab]` Equivalent to the character class `[ab]` in a regex.
  ///
  /// @param str The glob expression.
  ///
  /// @returns A regex for *str*.
  static regex glob(std::string const& str);

  /// Default-constructs an empty regex.
  regex() = default;

  /// Constructs a regex from a VAST string.
  /// @param str The regular expression string.
  regex(string str);

  regex(regex const& other) = default;
  regex(regex&& other) = default;
  regex& operator=(regex const& other) = default;
  regex& operator=(regex&& other) = default;

  /// Matches a string against the regex.
  /// @param str The string to match.
  /// @returns `true` if the regex matches exactly *str*.
  template <typename String>
  bool match(String const& str) const
  {
    return std::regex_match(str.begin(), str.end(), rx_);
  }

  /// Matches a string against the regex.
  /// @param str The string to match.
  /// @param f A function to invoke on each captured submatch.
  /// @returns `true` if the regex matches *str*.
  bool match(std::string const& str,
             std::function<void(std::string const&)> f) const;

  /// Searches a pattern in a string.
  /// @param str The string to search.
  /// @returns `true` if the regex matches inside *str*.
  template <typename String>
  bool search(String const& str) const
  {
    return std::regex_search(str.begin(), str.end(), rx_);
  }

private:
  std::regex rx_;
  string str_;

private:
  friend access;

  void serialize(serializer& sink) const;
  void deserialize(deserializer& source);

  template <typename Iterator>
  friend trial<void> print(regex const& rx, Iterator&& out)
  {
    *out++ = '/';

    auto t = print(rx.str_, out);
    if (! t)
      return t.error();

    *out++ = '/';

    return nothing;
  }

  template <typename Iterator>
  friend trial<void> parse(regex& rx, Iterator& begin, Iterator end)
  {
    if (*begin != '/')
      return error{"regex did not begin with a '/'"};

    auto t = parse<string>(begin, end);
    if (! t)
      return t.error();

    if (t->empty() || (*t)[t->size() - 1] != '/')
      return error{"regex did not end with a '/'"};

    rx = regex{t->thin("/", "\\")};
    return nothing;
  }

  friend bool operator==(regex const& x, regex const& y);
  friend bool operator<(regex const& x, regex const& y);
};

} // namespace vast

#endif
