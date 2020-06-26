#include "vast/schedule.hpp"

namespace vast {

size_t schedule::size() {
  return queue_.size();
}

uuid schedule::next() {
  auto result = queue_.front();
  queue_.pop_front();
  return result;
}

void schedule::add(const uuid& query_id, const uuid& partition_id) {
  auto exists
    = std::find(queue_.begin(), queue_.end(), partition_id) != queue_.end();
  if (!exists)
    queue_.push_back(partition_id);
}

} // namespace vast
