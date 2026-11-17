//
//  ContactManifold.h
//  EklavyaEngine
//
//  How two bodies touch this frame. Produced by the CollisionSystem *Vs* functions,
//  stored and solved by ContactConstraint.
//

#ifndef INC_CONTACT_MANIFOLD_H
#define INC_CONTACT_MANIFOLD_H

#include <glm/glm.hpp>
#include <vector>

namespace Eklavya::Physics
{
	struct ContactPoint
	{
		glm::vec3 position{0.0f}; // world space
		float depth = 0.0f; // > 0 when penetrating
	};

	struct ContactManifold
	{
		glm::vec3 normal{0.0f}; // unit length, points from body A to body B, shared by every point
		std::vector<ContactPoint> points; // 1 for sphere contacts, up to 4 for box vs box
	};
} // namespace Eklavya::Physics

#endif
