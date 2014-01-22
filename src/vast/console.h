#ifndef VAST_CONSOLE_H
#define VAST_CONSOLE_H

#include <deque>
#include "vast/actor.h"
#include "vast/cow.h"
#include "vast/expression.h"
#include "vast/file_system.h"
#include "vast/individual.h"
#include "vast/util/command_line.h"

namespace vast {

/// A console-based, interactive query client.
struct console : actor<console>
{
  struct options
  {
    uint64_t batch_size = 10;
    bool auto_follow = true;
  };

  /// A stream of events representing a result. One can add events to the
  /// result and seek forward/backward.
  class result : intrusive_base<result>, public individual
  {
  public:
    /// Default-constructs a result.
    result() = default;

    /// Constructs a result from a valid AST.
    result(expr::ast ast);

    /// Saves the current result state to a given directory.
    /// @param p The directory to save the results under.
    /// @returns `true` on success.
    bool save(path const& p) const;

    /// Loads the result state from a given directory.
    /// @param p The directory to load the results from.
    /// @returns `true` on success.
    bool load(path const& p);

    /// Adds an event to this result.
    /// @param e The event to add.
    void add(cow<event> e);

    /// Applies a function to a given number of existing events and advances
    /// the internal position.
    ///
    /// @param n The number of events to apply *f* to.
    ///
    /// @param f The function to apply to the *n* next events.
    ///
    /// @returns The number of events that *f* has been applied to.
    size_t apply(size_t n, std::function<void(event const&)> f);

    /// Adjusts the stream position.
    /// @param n The number of events.
    /// @returns The number of events seeked.
    size_t seek_forward(size_t n);

    /// Adjusts the stream position.
    /// @param n The number of events.
    /// @returns The number of events seeked.
    size_t seek_backward(size_t n);

    /// Retrieves the AST of this query result.
    expr::ast const& ast() const;

    /// Retrieves the number of events in the result.
    /// @returns The number of all events.
    size_t size() const;

  private:
    using pos_type = uint64_t;

    expr::ast ast_;
    pos_type pos_ = 0;
    std::deque<cow<event>> events_;

  private:
    friend access;
    void serialize(serializer& sink) const;
    void deserialize(deserializer& source);
  };

  enum print_mode
  {
    error,
    query,
    warn
  };

  /// Spawns the console client.
  /// @param search The search actor the console interacts with.
  /// @param dir The directory where to save state.
  console(cppa::actor_ptr search, path dir);

  void act();
  char const* description() const;

  void show_prompt(size_t ms = 100);
  std::ostream& print(print_mode mode);

  path dir_;
  std::vector<intrusive_ptr<result>> results_;
  std::map<cppa::actor_ptr, intrusive_ptr<result>> active_;
  std::pair<cppa::actor_ptr, intrusive_ptr<result>> current_;
  cppa::actor_ptr search_;
  util::command_line cmdline_;
  options opts_;
  bool follow_mode_ = false;
};

} // namespace vast

#endif
