#include <concurrent/static_thread_pool.hpp>

namespace concurrent
{

StaticThreadPool::StaticThreadPool(std::size_t workers_num)
{
  StartWorkers(workers_num);
}

void StaticThreadPool::SubmitTask(TaskType task)
{
  tasks_.Put(task);
}

void StaticThreadPool::Join()
{
  for (auto& worker : workers_) {
    tasks_.Put({}); // Poison pill
  }
  for (auto& worker : workers_) {
    worker.join();
  }
}

void StaticThreadPool::StartWorkers(std::size_t workers_num)
{
  for (std::size_t i = 0; i < workers_num; i++)
  {
    workers_.emplace_back([this]() { WorkerRoutine(); });
  }
}

void StaticThreadPool::WorkerRoutine()
{
  while (true)
  {
    auto task = tasks_.Take();
    if (!task) {
      break;
    }
    task();
  }
}

} // namespace concurrent