#include <concurent/mutex/mutex.hpp>

namespace concurrent::mutexes {
namespace tas {

void SpinLock::Lock() {
  /**
   * Если locked изменилась с true на false, то вызвавший Lock()
   * поток может зайти в критическую секцию.
   */
  while (locked_.exchange(true)) {
    /**
     * Уходим в конец очереди планировщика ОС, чтобы не греть камушек.
     */
    std::this_thread::yield();
  }
}

void SpinLock::Unlock() {
  /**
   * Выходя из критической секции, записываем в locked_ false.
   */
  locked_.store(false);
}

}  // namespace tas


namespace tickets {

void SpinLock::Lock() {
  /**
   * Атомарно берем значение переменной next_free_ticket
   * Сохраняем его в my_ticket
   * Увеличиваем next_free_ticket_ на 1. Тем самым формируя очередь
   */
  size_t my_ticket = next_free_ticket_.fetch_add(1);
  while (my_ticket != owner_ticket_.load()) {
    /**
     * Уходим в конец очереди планировщика ОС, чтобы не греть камушек.
     */
    std::this_thread::yield();
  }
}

void SpinLock::Unlock() {
  /**
   * Передача управления следующему потоку.
   */
  owner_ticket_.fetch_add(1);
}

} // namespace tickets


}  // namespace concurrent::mutexes