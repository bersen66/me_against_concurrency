#pragma once

namespace concurrent
{

struct Noncopyable
{
  Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
  Noncopyable(Noncopyable&&) = default;
  virtual ~Noncopyable() = default;
};

} // namespace concurrent
