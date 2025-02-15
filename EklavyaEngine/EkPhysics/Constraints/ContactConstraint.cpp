//
//  ContactConstraint.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#include "ContactConstraint.h"
#include <EkPhysics/EkBody.h>

namespace Eklavya::Physics
{

	ContactConstraint::ContactConstraint(EkBody& bodyA, EkBody& bodyB) : mBodyA(bodyA), mBodyB(bodyB)
	{
	}
	void ContactConstraint::Solve(float dt)
	{
		for (CollisionPointInfo& collisionPointInfo : mCollisionPoints)
		{
			if (dt <= 0.0)
			{
				return;
			}

			float invMass0 = mBodyA.GetInverseMass();
			float invMass1 = mBodyB.GetInverseMass();

			// Both objects are non movable
			if ((invMass0 + invMass1) == 0.0)
				return;

			glm::vec3 r0 = collisionPointInfo.location - mBodyA.GetPosition();
			glm::vec3 r1 = collisionPointInfo.location - mBodyB.GetPosition();

			glm::vec3 v0 = mBodyA.GetVelocity() + glm::cross(mBodyA.GetAngularVel(), r0);
			glm::vec3 v1 = mBodyB.GetVelocity() + glm::cross(mBodyB.GetAngularVel(), r1);

			// Relative Velocity
			glm::vec3 dv = v0 - v1;

			// NORMAL Impulse Code

			// Compute Normal Impulse
			float vn = glm::dot(dv, collisionPointInfo.normal);

			// Works out the bias to prevent Prevents sinking!
			const float allowedPenetration = 0.1f;
			const float biasFactor = 0.1f; // 0.1 to 0.3
			float       biasFactorValue = biasFactor;

			float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
			float bias = biasFactorValue * inv_dt * glm::max(0.0f, collisionPointInfo.peneteration - allowedPenetration);

			float kNormal = invMass0 + invMass1 +

			                glm::dot(collisionPointInfo.normal,
			                         glm::cross((glm::cross(r0, collisionPointInfo.normal) * mBodyA.I()), r0) +
			                             glm::cross((glm::cross(r1, collisionPointInfo.normal) * mBodyB.I()), r1));

			float massNormal = 1.0f / kNormal;

			float dPn = massNormal * (-vn + bias);

			dPn = glm::max(dPn, 0.0f);

			// Apply normal contact impulse
			glm::vec3 P = dPn * collisionPointInfo.normal;

			mBodyA.ApplyImpulse(P, r0);
			mBodyB.ApplyImpulse(-P, r1);

			// Tangent friction impulse
			{
				glm::vec3 tangent = glm::vec3(0, 0, 0);
				tangent = dv - (glm::dot(dv, collisionPointInfo.normal) * collisionPointInfo.normal);
				if (fabs(glm::length(tangent)) < FLT_EPSILON)
					return;
				tangent = glm::normalize(tangent);

				float kTangent = invMass0 + invMass1 +

				                 glm::dot(tangent, glm::cross((glm::cross(r0, tangent) * mBodyA.I()), r0) + glm::cross((glm::cross(r1, tangent) * mBodyB.I()), r1));

				float massTangent = 1.0f / kTangent;

				float vt = glm::dot(dv, tangent);
				float dPt = massTangent * (-vt);

				float maxPt = 0.6f * dPn;
				dPt = glm::clamp(dPt, -maxPt, maxPt);

				// Apply contact impulse
				glm::vec3 P = dPt * tangent;

				mBodyA.ApplyImpulse(P, r0);
				mBodyB.ApplyImpulse(-P, r1);
			}
		}
	}
} // namespace Eklavya::Physics
