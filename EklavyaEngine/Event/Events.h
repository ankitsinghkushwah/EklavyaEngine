#pragma once

#include "EventType.h"
#include <memory>
#include <glm/glm.hpp>
#include <vector>

class IEventData
{
  public:
	EEventType GetType()
	{
		return m_Type;
	}

  protected:
	EEventType m_Type;
};
