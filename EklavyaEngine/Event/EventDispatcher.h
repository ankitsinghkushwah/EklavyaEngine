#pragma once

#include "Events.h"
#include "EventType.h"
#include <vector>
#include <functional>

using Callback = std::function<void(std::shared_ptr<IEventData>)>;
using EventDataSharedPtr = std::shared_ptr<IEventData>;

class EventDispatcher {
public:
  static EventDispatcher &GetInstance() {
    static EventDispatcher instance;
    return instance;
  }

  void AddListener(Callback method, EEventType eventType);
  void RemoveListener(Callback method, EEventType eventType = EEventType::LAST);
  void TriggerEvent(EEventType eventType, EventDataSharedPtr eventData);

  ~EventDispatcher();

private:
  EventDispatcher();
  std::vector<std::vector<Callback>> m_Listeners;
};

template <typename Type>
using FuncPtr = void (Type::*)(std::shared_ptr<IEventData>);

template <typename Type>
std::function<void(std::shared_ptr<IEventData>)>
MakeDelegate(FuncPtr<Type> callback, Type *owner) {
  return std::bind(callback, owner, std::placeholders::_1);
}

template <typename Type>
inline void RegisterToEvent(Type *owner, FuncPtr<Type> callback,
                            EEventType eventType) {
  EventDispatcher::GetInstance().AddListener(MakeDelegate(callback, owner),
                                             eventType);
}

template <typename Type>
inline void UnRegisterToEvent(Type *owner, FuncPtr<Type> callback,
                              EEventType eventType) {
  EventDispatcher::GetInstance().RemoveListener(MakeDelegate(callback, owner),
                                                eventType);
}