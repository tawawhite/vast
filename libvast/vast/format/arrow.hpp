#ifndef VAST_FORMAT_ARROW_HPP

#define VAST_FORMAT_ARROW_HPP

#include <string>

#include "plasma/client.h"

#include "arrow/api.h"
#include "arrow/builder.h"

#include "vast/data.hpp"
#include "vast/expected.hpp"
#include "vast/type.hpp"

namespace vast {

class event;

namespace format {
namespace arrow {

/// Converts events into Arrow Record batches and writes them into a Plasma
/// store.

class writer {
public:
  writer() = default;

  /// Constructs an Arrow writer that connects to a (local) plasma store.
  /// @param plasma_socket The path to the local Plasma listening socket.
  writer(const std::string& plasma_socket);

  ~writer();

  expected<void> write(const std::vector<event>& xs);

  expected<void> write(const event& x);

  expected<void> flush();

  const char* name() const;

  /// Checks whether the writer is connected to the Plasma store.
  /// @returns `true` if the connection to the Plasma store is alive.
  bool connected() const;

private:
  expected<plasma::ObjectID> make_object(const void* data, size_t size);
  bool connected_;
  plasma::PlasmaClient plasma_client_;
  std::vector<event> buffer_;
};
struct convert_visitor {
  using result_type = std::shared_ptr<::arrow::Field>;
  std::vector<result_type> schema_vector_period = {
    ::arrow::field("num", ::arrow::int64()),
    ::arrow::field("denum", ::arrow::int64()),
  };
  // Timespan
  std::vector<result_type> schema_vector_timespan = {
    ::arrow::field("rep", ::arrow::int64()),
    ::arrow::field("period", ::arrow::struct_(schema_vector_period)),
  };
  inline result_type operator()(const boolean_type&) {
    return ::arrow::field("bool", ::arrow::boolean());
  }
  inline result_type operator()(const count_type&) {
    return ::arrow::field("count", ::arrow::uint64());
  }
  inline result_type operator()(const integer_type&) {
    return ::arrow::field("integer", ::arrow::int64());
  }
  inline result_type operator()(const real_type&) {
    return ::arrow::field("real", ::arrow::float64());
  }
  inline result_type operator()(const string_type&) {
    return ::arrow::field("string", std::make_shared<::arrow::StringType>());
  }
  inline result_type operator()(const pattern_type&) {
    return ::arrow::field("pattern", std::make_shared<::arrow::StringType>());
  }
  inline result_type operator()(const address_type&) {
    return ::arrow::field("address",
                          std::make_shared<::arrow::FixedSizeBinaryType>(16));
  }
  inline result_type operator()(const port_type&) {
    std::vector<result_type> schema_vector_port = {
      ::arrow::field("port_type", ::arrow::int8()),
      ::arrow::field("mask", ::arrow::int16()),
    };
    auto port_struct =
      std::make_shared<::arrow::StructType>(schema_vector_port);
    return ::arrow::field("port", port_struct);
  }
  inline result_type operator()(const subnet_type&) {
    std::vector<result_type> schema_vector_subnet = {
      ::arrow::field("address",
                     std::make_shared<::arrow::FixedSizeBinaryType>(16)),
      ::arrow::field("mask", ::arrow::int8()),
    };
    auto subnet_struct =
      std::make_shared<::arrow::StructType>(schema_vector_subnet);
    return ::arrow::field("subnet", subnet_struct);
  }
  inline result_type operator()(const timespan_type&) {
    auto timespan_struct =
      std::make_shared<::arrow::StructType>(schema_vector_timespan);
    return ::arrow::field("timespan", timespan_struct);
  }
  inline result_type operator()(const timestamp_type&) {
    auto timespan_struct =
      std::make_shared<::arrow::StructType>(schema_vector_timespan);
    // Timepoint
    std::vector<result_type> schema_vector_timepoint = {
      ::arrow::field("clock", ::arrow::int64()),
      ::arrow::field("timespan", timespan_struct),
    };
    auto timepoint_struct =
      std::make_shared<::arrow::StructType>(schema_vector_timepoint);
    return ::arrow::field("timepoint", timepoint_struct);
  }
  template <class T>
  result_type operator()(const T&) {
    return ::arrow::field("none", ::arrow::null());
  }
};

struct insert_visitor {
  ::arrow::ArrayBuilder* builder;
  ::arrow::RecordBatchBuilder* rbuilder;
  u_int64_t counter = 0;
  insert_visitor(::arrow::ArrayBuilder& b);
  insert_visitor(::arrow::RecordBatchBuilder& b);

  ::arrow::Status operator()(const record_type t, const std::vector<data> d);
  /*
  void operator()(const record_field t, const data d);
  void operator()(const string_type t, const data d);
  void operator()(const real_type t, const data d);
  void operator()(const integer_type t, const data d);
  void operator()(const count_type, const data d);
  void operator()(const boolean t, const data d);
*/
  template <class T1, class T2>
  ::arrow::Status operator()(const T1, const T2) {
    std::cout << typeid(T1).name() << " Wuff " << typeid(T2).name()
              << std::endl;
    return ::arrow::Status::OK();
  };
  /*
  inline
  ::arrow::Status operator()(const u_int64_t d) {
    std::cout << "count" << std::endl;
    auto cbuilder = static_cast<::arrow::UInt64Builder*>(builder);
    return cbuilder->Append(d);
  }
  */
  inline ::arrow::Status operator()(const type t, const data d) {
    std::cout << typeid(d).name() << std::endl;
    std::cout << "default" << std::endl;
    return ::arrow::Status::OK();
  }
  inline ::arrow::Status operator()(const none_type t, const none d) {
    std::cout << typeid(d).name() << std::endl;
    std::cout << "null" << std::endl;
    auto nbuilder = static_cast<::arrow::NullBuilder*>(builder);
    return nbuilder->AppendNull();
  }
  inline ::arrow::Status operator()(const string_type t, const std::string d) {
    std::cout << d << std::endl;
    std::cout << "string: " << builder->type()->ToString() << std::endl;
    auto nbuilder = static_cast<::arrow::StringBuilder*>(builder);
    return nbuilder->Append(d);
  }
  inline ::arrow::Status operator()(const boolean_type t, const bool d) {
    std::cout << "bool " << builder->type()->name() << " " << d << std::endl;
    std::cout << d << std::endl;
    auto bbuilder = static_cast<::arrow::BooleanBuilder*>(builder);
    std::cout << "bool1" << std::endl;
    return bbuilder->Append(d);
  }
  inline ::arrow::Status operator()(const timestamp_type t, const timestamp d) {
    std::cout << "timestamp" << std::endl;
    return ::arrow::Status::OK();
  }
  inline ::arrow::Status operator()(const subnet_type t, const subnet d) {
    std::cout << "subnet" << std::endl;
    return ::arrow::Status::OK();
  }
  inline ::arrow::Status operator()(const address_type t, const address d) {
    std::cout << "address" << std::endl;
    return ::arrow::Status::OK();
  }
  inline ::arrow::Status operator()(const port_type t, const port d) {
    std::cout << "port" << std::endl;
    return ::arrow::Status::OK();
  }
  inline ::arrow::Status operator()(const vector_type t,
                                    const std::vector<data> d) {
    std::cout << "vector" << std::endl;
    return ::arrow::Status::OK();
  }
};
} // namespace arrow
} // namespace format
} // namespace vast

#endif