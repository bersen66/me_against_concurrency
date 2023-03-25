#pragma once

#include <optional>
#include <deque>
#include <memory>
#include <concurrent/blocking_queue.hpp>

namespace concurrent
{

/**
 * @brief Механизм обмена данными между thread-ами.
 * Канал удобно представить как две соединенные накрест.
 *         WRITE(A) -> READ(B)  -- UPLINE
 *         WRITE(B) -> READ(A)  -- DOWNLINE
 * Данные образуют очередь. Коммуникация осуществляется через Endpoint-ы
 * Между Endpoint-ами находится буфер, в котором
 * содержатся данные, до которых еще не дошла очередь.
 */
namespace detail
{
/**
 * @brief Хранит обмениваемые данные.
 * @tparam T - тип обмениваемых данных.
 */
template <typename T>
class ChannelStorage
{
public:
  /// @brief rule of five https://en.cppreference.com/w/cpp/language/rule_of_three
  ChannelStorage() noexcept = default;
  ChannelStorage(ChannelStorage&&) noexcept = default;
  ~ChannelStorage() = default;

  ChannelStorage(const ChannelStorage&) noexcept = delete;
  ChannelStorage& operator=(const ChannelStorage&) noexcept = delete;

  /**
   * @brief Переслать значение по UPLINE.
   * @param value
   */
  void PushToUpline(T&& value)
  {
    upline_.Put(std::move(value));
  }

  /**
   * @brief Переслать значение по DOWNLINE.
   * @param value
   */
  void PushToDownline(T&& value)
  {
    downline_.Put(std::move(value));
  }

  /**
   * @brief Считать значение из UPLINE.
   * Если сообщений нет, то вызывающий поток заблокируется
   * до тех пор, пока сообщение не появится. Выход из функции возможен только со значением.
   */
  T TakeUpline()
  {
    return upline_.Take();
  }

  /**
   * @brief Считать значение из DOWNLINE.
   * Если сообщений нет, то вызывающий поток заблокируется
   * до тех пор, пока сообщение не появится. Выход из функции возможен только со значением.
   */
  T TakeDownline()
  {
    return downline_.Take();
  }

  /**
   * @brief Попытаться считать значение из UPLINE.
   * Если сообщений нет, то вызывающий поток получит std::nullopt
   * Иначе - std::optional содержащее значение.
   */
  std::optional<T> TryTakeUpline()
  {
    return upline_.TryTake();
  }

  /**
   * @brief Попытаться считать значение из DOWNLINE.
   * Если сообщений нет, то вызывающий поток получит std::nullopt
   * Иначе - std::optional содержащее значение.
   */
  std::optional<T> TryTakeDownline()
  {
    return downline_.TryTake();
  }

private:
  ::concurrent::BlockingQueue<T> upline_;
  ::concurrent::BlockingQueue<T> downline_;
};

/// @brief Умный указатель на ChannelStorage
template <typename T>
using ChannelPtr = std::shared_ptr<detail::ChannelStorage<T>>;

} // namespace detail

/**
 * @brief Конечная точка, через которую осуществляется коммуникация.
 * @tparam T - тип пересылаемых значений.
 */
template <typename T>
class ChannelEndpoint
{
private:
  /// @brief Тип линии канала, на которой будет осуществляться коммуникация
  enum class Line
  {
    UPLINE,
    DOWNLINE,
  };

  /// @brief Конструктор
  ChannelEndpoint(detail::ChannelPtr<T> channel_ptr, Line role) noexcept
      : channel_(channel_ptr) /// данные канала
      , line_(role)           /// линия по которой идет общение
  {
  }

public:
  ChannelEndpoint(const ChannelEndpoint&) = default;
  ChannelEndpoint& operator=(const ChannelEndpoint&) = default;
  ~ChannelEndpoint() = default;

  /**
   * @brief Записать значение в канал
   * @param value - отправляемое значение
   */
  void SendData(T&& value) const noexcept
  {
    switch (line_)
    {
    case Line::UPLINE:
      channel_->PushToUpline(std::move(value));
      break;
    case Line::DOWNLINE:
      channel_->PushToDownline(std::move(value));
      break;
    }
  }

  /**
   * @brief Блокирующее чтение. Если сообщений нет, то поток блокируется.
   * @param value - отправляемое значение
   */
  T BlockingRead() const noexcept
  {
    switch (line_)
    {
    case Line::UPLINE:
      return channel_->TakeDownline();
    case Line::DOWNLINE:
      return channel_->TakeUpline();
    }
  }

  /**
   * @brief Не блокирующее чтение.
   * Если сообщений нет - вернет std::nullopt.
   * Если есть - std::optional<T>
   */
  std::optional<T> TryRead() const noexcept
  {
    switch (line_)
    {
    case Line::UPLINE:
      return channel_->TryTakeDownline();
    case Line::DOWNLINE:
      return channel_->TryTakeUpline();
    }
    return std::nullopt;
  }

  /// @brief Создание канала.
  template <typename U>
  friend auto MakeChannel();

private:
  mutable detail::ChannelPtr<T> channel_;
  const Line line_;
};

/// ПОЖАЛУЙСТА СОЗДАВАЙТЕ КАНАЛЫ ТОЛЬКО ВЫЗОВОМ ЭТОЙ ФУНКЦИИ!
template <typename T>
auto MakeChannel()
{
  auto channel_ptr = std::make_shared<detail::ChannelStorage<T>>();
  return std::make_pair<ChannelEndpoint<T>>(ChannelEndpoint<T>(channel_ptr, ChannelEndpoint<T>::Line::UPLINE),
                                            ChannelEndpoint<T>(channel_ptr, ChannelEndpoint<T>::Line::DOWNLINE));
}

} // namespace concurrent
