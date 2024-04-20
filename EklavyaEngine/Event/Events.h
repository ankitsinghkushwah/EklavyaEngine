#pragma once

#include "EventType.h"
#include <memory>
#include "../Components/GameActor.h"
#include <glm/glm.hpp>
#include <vector>

class IEventData {
public:
  EEventType GetType() { return m_Type; }

protected:
  EEventType m_Type;
};

class EventActorCreated : public IEventData {
public:
  std::shared_ptr<GameActor> m_Actor;
};

class EventActorDestroyed : public IEventData {
public:
  std::shared_ptr<GameActor> m_GameActor;
};

class EventOnCollisionEnter : public IEventData {
public:
  unsigned ActorID;
};

class EventOnCollisionExit : public IEventData {
public:
  unsigned ActorID;
};
