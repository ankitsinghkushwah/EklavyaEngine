#include "EventDispatcher.h"
#include <cassert>
#include <algorithm>

EventDispatcher::~EventDispatcher() {}

EventDispatcher::EventDispatcher() {
  m_Listeners.reserve(EEventType::LAST);
  for (int type = 0; type < EEventType::LAST; ++type) {
    m_Listeners.push_back(std::vector<Callback>());
    m_Listeners[type].reserve(10);
  }
}

void EventDispatcher::AddListener(Callback listener, EEventType eventType) {
  assert(eventType >= EEventType::FIRST && eventType < EEventType::LAST);
  auto iter =
      std::find_if(m_Listeners[eventType].begin(), m_Listeners[eventType].end(),
                   [&](Callback &one) -> bool { return &one == &listener; });
  if (iter != m_Listeners[eventType].end())
    return;
  m_Listeners[eventType].push_back(listener);
}

void EventDispatcher::RemoveListener(Callback listener, EEventType eventType) {
  assert(eventType >= EEventType::FIRST && eventType < EEventType::LAST);
  auto iter =
      std::find_if(m_Listeners[eventType].begin(), m_Listeners[eventType].end(),
                   [&](Callback &one) -> bool { return &one == &listener; });
  if (iter == m_Listeners[eventType].end())
    return;
  m_Listeners[eventType].erase(iter);
}

void EventDispatcher::TriggerEvent(EEventType eventType,
                                   EventDataSharedPtr eventData) {
  assert(eventType >= EEventType::FIRST && eventType < EEventType::LAST);

  for (auto callback : m_Listeners[eventType])
    callback(eventData);
}