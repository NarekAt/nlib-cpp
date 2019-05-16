#ifndef NLIB_CONTAINER_INTERVAL_MAP_H_
#define NLIB_CONTAINER_INTERVAL_MAP_H_

#include <cstddef>
#include <limits>
#include <map>

namespace nlib {

template <typename K, typename V>
class IntervalMap {
 public:
  IntervalMap(V const& val) {
    map_.insert(map_.end(), {std::numeric_limits<K>::lowest(), val});
  }

  // Set [key_begin..key_end) to |val|.
  // Overwrite previous values in this interval.
  void assign(K const& key_begin, K const& key_end, V const& val) {
    if (!(key_begin < key_end)) {
      return;
    }

    auto is_equal = [](auto it1, auto it2) {
      return !(it1->first < it2->first) && !(it2->first < it1->first);
    };

    auto insert = [this, &is_equal](K key) {
      auto result = map_.insert({key, V()});
      // The new value of the existing key should be set to the
      // same as the previous entry. Note that this will make
      // key_end to be excluded from the interval as well.
      if (result.second && !is_equal(result.first, map_.begin())) {
        auto prev_entry = result.first;
        --prev_entry;
        result.first->second = prev_entry->second;
      }

      return result.first;
    };

    auto entry_begin = insert(key_begin);
    auto entry_end = insert(key_end);
    entry_begin->second = val;

    // Remove all entries within the range (entry_begin, entry_end).
    while (true) {
      auto next_in_begin_to_end = entry_begin;
      ++next_in_begin_to_end;

      if (!is_equal(next_in_begin_to_end, entry_end)) {
        map_.erase(next_in_begin_to_end);
      } else {
        break;
      }
    }

    // Remove duplicate consecutive entries (made by insert()) from
    // left and right side of the given 'entry'.
    auto make_canonical = [this, &is_equal](auto entry) {
      if (entry == map_.end()) {
        return;
      }

      auto left = entry;
      auto right = entry;
      if (!is_equal(left, map_.begin())) {
        --left;
        if (!(left->second == entry->second)) {
          left = right;
        } else {
          map_.erase(right);
          right = left;
        }
      }

      if (++right != map_.end() && right->second == left->second) {
        map_.erase(right);
      }
    };

    make_canonical(entry_begin);
    // 'entry_end' may be invalided by the previous call,
    // so update the iterator.
    entry_end = map_.lower_bound(key_end);
    make_canonical(entry_end);
  }

  V const& operator[](K const& key) const {
    return (--map_.upper_bound(key))->second;
  }

 private:
  std::map<K, V> map_;
};

}  // namespace nlib

#endif