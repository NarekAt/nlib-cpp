#ifndef NLIB_CONCURRENT_CONCURRENT_QUEUE_H
#define NLIB_CONCURRENT_CONCURRENT_QUEUE_H

#include <condition_variable>
#include <memory>
#include <mutex>

namespace nlib::concurrent {

template <class T>
class ConcurrentQueue {
 public:
  ConcurrentQueue() : head_(std::make_unique<Node>()), tail_(head_.get()) {}

  bool TryPop(T* value) {
    auto old_head = PopHead();
    if (old_head) {
      *value = old_head->data;
      return true;
    }

    return false;
  }

  bool Front(T* value) const {
    std::lock_guard<std::mutex> head_lock(head_mutex_);

    if (head_.get() == GetTail()) {
      return false;
    }

    *value = head_->data;
    return true;
  }

  void WaitAndPop(T* value) {
    std::unique_lock<std::mutex> head_lock(head_mutex_);
    data_ready_.wait(head_lock, [&] { return head_.get() != GetTail(); });

    *value = head_->data;
    PopHead();
  }

  void Push(const T& new_value) {
    auto new_node = std::make_unique<Node>(new_value);

    Node* const new_tail = new_node.get();

    {
      std::lock_guard<std::mutex> tail_lock(tail_mutex_);
      tail_->data = new_value;
      tail_->next = std::move(new_node);
      tail_ = new_tail;
    }

    // If we leave the mutex locked across the call to notify_one(), then if the
    // notified thread wakes up before the mutex has been unlocked, it will have
    // to wait for the mutex.
    data_ready_.notify_one();
  }

  bool Empty() {
    std::lock_guard<std::mutex> head_lock(head_mutex_);
    return head_.get() == GetTail();
  }

 private:
  struct Node {
    Node() = default;
    explicit Node(const T& val) : data(val), next(nullptr) {}

    T data;
    std::unique_ptr<Node> next;
  };

  Node* GetTail() {
    std::lock_guard<std::mutex> tail_lock(tail_mutex_);
    return tail_;
  }

  std::unique_ptr<Node> PopHead() {
    std::lock_guard<std::mutex> head_lock(head_mutex_);

    if (head_.get() == GetTail()) {
      return nullptr;
    }

    auto old_head = std::move(head_);
    head_ = std::move(old_head->next);
    return old_head;
  }

 private:
  std::unique_ptr<Node> head_;
  Node* tail_;
  mutable std::mutex head_mutex_;
  mutable std::mutex tail_mutex_;
  std::condition_variable data_ready_;
};

}  // namespace nlib::concurrent

#endif