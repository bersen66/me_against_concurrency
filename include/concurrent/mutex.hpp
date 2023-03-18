#pragma once

#include <array>
#include <atomic>
#include <concurrent/noncopyable.hpp>
#include <thread>

namespace concurrent
{
namespace tas
{
/**
 * Test-and-Set SpinLock
 * Sometimes contention
 */
class SpinLock : private Noncopyable
{
public:
  void Lock();
  void Unlock();

  // Support of https://en.cppreference.com/w/cpp/named_req/BasicLockable
  void lock();
  void unlock();

private:
  std::atomic<bool> locked_{false};
};

} // namespace tas

namespace tickets
{
/**
 * Ticket SpinLock
 */
class SpinLock : private Noncopyable
{
public:
  void Lock();
  void Unlock();

  // Support of https://en.cppreference.com/w/cpp/named_req/BasicLockable
  void lock();
  void unlock();

private:
  std::atomic<std::size_t> next_free_ticket_{0};
  std::atomic<std::size_t> owner_ticket_{0};
};

} // namespace tickets

template <typename MutexType>
class LockGuard : private Noncopyable
{
public:
  LockGuard(MutexType& mutex)
      : mutex_(mutex)
  {
    mutex_.lock();
  }

  ~LockGuard()
  {
    mutex_.unlock();
  }

private:
  MutexType& mutex_;
};



} // namespace concurrent
