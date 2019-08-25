#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <vector>

class DefaultResizePolicy {
 public:
  size_t operator()(size_t old_size) { return 2 * old_size; }
};

template <typename Key, typename Hash = std::hash<Key>,
          typename ResizePolicy = DefaultResizePolicy>
class HashTable {
 public:
  // Type definitions.
  using key_type = Key;
  using size_type = size_t;
  using hasher = Hash;

 public:
  HashTable() = default;
  explicit HashTable(size_t buckets_count)
      : buckets_count_(buckets_count),
        table_(Table(buckets_count, std::list<Key>())) {}

  size_t size() const { return size_; }

  bool empty() const { return !size(); }

  size_t num_buckets() const { return buckets_count_; }

  // State modifiers.

  void set_load_factor(float load_factor) { load_factor_ = load_factor; }

  void insert(Key key) {
    assert(buckets_count_);

    size_t slot_index = Hash()(key) % buckets_count_;
    table_[slot_index].push_back(key);

    if (++size_ >= buckets_count_ * load_factor_) {
      resize();
    }
  }

  bool contains(Key key) {
    size_t slot_index = Hash()(key) % buckets_count_;
    const auto& slot = table_[slot_index];

    if (slot.empty()) {
      return false;
    }

    return (std::find(slot.begin(), slot.end(), key) != slot.end());
  }

  void remove(Key key) {
    assert(buckets_count_);

    size_t slot_index = Hash()(key) % buckets_count_;
    auto& slot = table_[slot_index];

    size_t size_before_remove = slot.size();
    slot.remove(key);
    bool is_removed = size_before_remove > slot.size();
    if (is_removed) {
      --size_;
    }
  }

 private:
  // Resets size of table according to ResetPolicy and
  // iterates over old table to add keys to a new
  // table (re-hashing).
  void resize() {
    assert(buckets_count_);

    buckets_count_ = DefaultResizePolicy()(buckets_count_);

    Table old_table = table_;
    table_.clear();
    size_ = 0;
    table_.resize(buckets_count_);
    for (const auto& slot : old_table) {
      for (const auto& key : slot) {
        insert(key);
      }
    }
  }

 private:
  size_t buckets_count_;
  using Table = std::vector<std::list<Key>>;
  Table table_;
  // Total number of entries in table;
  size_t size_ = 0;
  // Controls the resizing.
  // If size_ > buckets_count_ * load_factor_,
  // a resize will be performed.
  // 75 % is a commonly used load factor in general.
  float load_factor_ = 0.75f;
};
