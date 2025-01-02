//
//  ContactConstraint.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef ContactConstraint_h
#define ContactConstraint_h

#include "Constraint.h"
#include <vector>
#include <glm/glm.hpp>

namespace Eklavya::Physics
{

	class EkBody;

	//CONTACT CONSTRAINT
	struct CollisionPointInfo
	{
		CollisionPointInfo() = default;
		CollisionPointInfo(const glm::vec3& loc, const glm::vec3& norm, float pen) : location(loc), normal(norm), peneteration(pen)
		{
		}
		glm::vec3 location;
		glm::vec3 normal;
		float     peneteration;
	};

	class ContactConstraint : public Constraint
	{
	  public:
		ContactConstraint(EkBody& bodyA, EkBody& bodyB);

		const std::vector<CollisionPointInfo>& GetCollisionPoints() const
		{
			return mCollisionPoints;
		}
		bool BodiesMatch(const EkBody& bodyA, const EkBody& bodyB) const
		{
			return &bodyA == &mBodyA && &bodyB == &mBodyB;
		}

		void AddCollisionPoint(glm::vec3 point, glm::vec3 collisionNormal, float peneteration)
		{
			mCollisionPoints.emplace_back(point, collisionNormal, peneteration);
		}
		void PreSolve() override
		{
		}
		void Solve(float dt) override;
		void PostSolve() override
		{
		}

	  private:
		EkBody& mBodyA;
		EkBody& mBodyB;

		std::vector<CollisionPointInfo> mCollisionPoints;
	};
} // namespace Eklavya::Physics
#endif /* ContactConstraint_hpp */
