#pragma once

#include <concurrent/blocking_queue.hpp>
#include <concurrent/noncopyable.hpp>

#include <functional>
#include <thread>
#include <vector>

namespace concurrent
{

class StaticThreadPool : private Noncopyable
{
public:
  using TaskType = std::function<void()>;

  explicit StaticThreadPool(std::size_t workers_num);

  void SubmitTask(TaskType task);

  void Join();

private:
  void StartWorkers(std::size_t workers_num);

  void WorkerRoutine();

private:
  std::vector<std::thread> workers_;
  BlockingQueue<TaskType> tasks_;
};

} // namespace concurrent