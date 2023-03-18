#pragma once
#include <concurrent/mutex.hpp>
#include <deque>
#include <mutex>
#include <condition_variable>

namespace concurrent
{


// Unbounded Blocking Multi-Producer/Multi-Consumer Queue
template <typename T>
class BlockingQueue
{
public:

  // Thread role: consumer
  T Take()
  {
    /**
     * Зашедший сюда поток
     */
    std::unique_lock lock(mutex_);
    while (buffer_.empty()) {
      /**
       * 1) Отпускаем мьютекс
       * 2) Ждем
       * 3) Выходя из wait перезахватываем мьютекс
       */
      not_empty_.wait(lock);
    }
    return TakeLocked();
  }

  // Thread role: producer
  void Put(T value)
  {
    LockGuard guard(mutex_);
    buffer_.push_back(std::move(value));
    not_empty_.notify_one();
  }

private:
  T TakeLocked() {
    auto result = std::move(buffer_.front());
    buffer_.pop_front();
    return result;
  }
private:
  std::deque<T> buffer_; // protected by mutex_
  std::condition_variable not_empty_;
  mutable std::mutex mutex_;
};

} // namespace concurrent
