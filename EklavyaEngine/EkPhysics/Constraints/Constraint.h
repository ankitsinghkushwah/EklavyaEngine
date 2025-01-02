#ifndef INC_CONSTRAINT_H_
#define INC_CONSTRAINT_H_

#include "EkBody.h"
#include <glm/glm.hpp>

namespace Eklavya::Physics
{
	class Constraint
	{
	  public:
		virtual void PreSolve() = 0;
		virtual void Solve(float deltaTime) = 0;
		virtual void PostSolve() = 0;
	};

} // namespace Eklavya::Physics

#endif
