#pragma once

#include <array>
#include <atomic>
#include <thread>

namespace concurrent::mutexes {
namespace tas {
/**
 * Test-and-Set SpinLock
 * Sometimes contention
 * Has problem with caches coherence
 */
class SpinLock {
 public:
  void Lock();
  void Unlock();

 private:
  std::atomic<bool> locked_{false};
};

}  // namespace tas

namespace tickets {
/**
 * Ticket SpinLock
 */
class SpinLock {
 public:
  void Lock();
  void Unlock();

 private:
  std::atomic<std::size_t> next_free_ticket_{0};
  std::atomic<std::size_t> owner_ticket_{0};
};

}  // namespace tickets

}  // namespace concurrent::mutexes
