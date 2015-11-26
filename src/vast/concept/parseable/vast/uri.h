#ifndef VAST_CONCEPT_PARSEABLE_VAST_URI_H
#define VAST_CONCEPT_PARSEABLE_VAST_URI_H

#include <algorithm>
#include <cctype>
#include <string>

#include "vast/concept/parseable/core.h"
#include "vast/concept/parseable/string.h"
#include "vast/concept/parseable/numeric/real.h"
#include "vast/uri.h"
#include "vast/util/string.h"

namespace vast {

// A parser for [query strings](https://en.wikipedia.org/wiki/Query_string).
struct uri_query_string_parser : parser<uri_query_string_parser> {
  using attribute = uri::query_type;

  static std::string query_unescape(std::string str) {
    std::replace(str.begin(), str.end(), '+', ' ');
    return util::percent_unescape(str);
  };

  static std::string percent_unescape(std::string str) {
    return util::percent_unescape(str);
  };

  static auto make() {
    using namespace parsers;
    auto query_key = +(printable - '=') ->* percent_unescape;
    auto query_ignore_char = '&'_p | '#' | ' ';
    auto query_value = +(printable - query_ignore_char) ->* query_unescape;
    auto query = query_key >> '=' >> query_value;
    auto query_string = query % '&';
    return query_string;
  }

  template <typename Iterator, typename Attribute>
  bool parse(Iterator& f, Iterator const& l, Attribute& a) const {
    static auto p = make();
    return p.parse(f, l, a);
  }
};

// A URI parser based on RFC 3986.
struct uri_parser : parser<uri_parser> {
  using attribute = uri;

  static auto make() {
    using namespace parsers;
    auto percent_unescape =  uri_query_string_parser::percent_unescape;
    auto scheme_ignore_char = ':'_p | '/';
    auto scheme = *(printable - scheme_ignore_char);
    auto host = *(printable - scheme_ignore_char);
    auto port = u16;
    auto path_ignore_char = '/'_p | '?' | '#' | ' ';
    auto path_segment = *(printable - path_ignore_char) ->* percent_unescape;
    auto fragment = *(printable - ' ');
    auto query = uri_query_string_parser{};
    auto uri
      =  ~(scheme >> ':')
      >> ~("//" >> host)
      >> ~(':' >> port)
      >> '/' >> path_segment % '/'
      >> ~('?' >> query)
      >> ~('#' >>  fragment)
      ;
    return uri;
  }

  template <typename Iterator>
  bool parse(Iterator& f, Iterator const& l, unused_type) const {
    static auto p = make();
    return p.parse(f, l, unused);
  }

  template <typename Iterator>
  bool parse(Iterator& f, Iterator const& l, uri& u) const {
    static auto p = make();
    auto t = std::tie(u.scheme, u.host, u.port, u.path, u.query, u.fragment);
    return p.parse(f, l, t);
  }
};

template <>
struct parser_registry<uri> {
  using type = uri_parser;
};

} // namespace vast

#endif