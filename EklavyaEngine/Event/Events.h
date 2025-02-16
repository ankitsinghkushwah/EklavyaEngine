#pragma once

#include "EventType.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class IEventData
{
  public:
	EEventType GetType() { return m_Type; }

  protected:
	EEventType m_Type;
};
