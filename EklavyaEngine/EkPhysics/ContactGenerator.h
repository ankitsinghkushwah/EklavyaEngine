//
//  ContactGenerator.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef ContactGenerator_h
#define ContactGenerator_h

#include "ContactConstraint.h"

namespace Eklavya::Physics
{
	class SphereColliderComponent;
	class BoxColliderComponent;
	class EkBody;

	namespace ContactGenerator
	{

		//Contact Generations
		bool SphereAndSphere(const SphereColliderComponent& sphereOne, const SphereColliderComponent& sphereTwo, CollisionPointInfo& newContact);
		bool SphereAndBox(const BoxColliderComponent& box, const SphereColliderComponent& sphere, CollisionPointInfo& newContact);

		bool BoxAndBox(const BoxColliderComponent& box1,
		               const BoxColliderComponent& box2,
		               std::vector<glm::vec3>&     collisionPoints,
		               float&                      penetration,
		               glm::vec3&                  collisionNormal);

		std::vector<ContactConstraint> CreateConstraints(std::vector<EkBody*>& bodies);

	} // namespace ContactGenerator
} // namespace Eklavya::Physics

#endif /* ContactGenerator_h */
