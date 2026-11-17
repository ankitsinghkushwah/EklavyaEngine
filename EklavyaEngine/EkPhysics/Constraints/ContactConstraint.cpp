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

	ContactConstraint::ContactConstraint(EkBody& bodyA, EkBody& bodyB, const ContactManifold& manifold)
		: mBodyA(bodyA), mBodyB(bodyB), mManifold(manifold)
	{
	}
	void ContactConstraint::Solve(float dt)
	{
		if (dt <= 0.0f)
			return;

		const float invMass0 = mBodyA.GetInverseMass();
		const float invMass1 = mBodyB.GetInverseMass();

		// Both objects are non movable
		if ((invMass0 + invMass1) == 0.0f)
			return;

		const glm::vec3 &normal = mManifold.normal;

		for (const ContactPoint& contact : mManifold.points)
		{
			glm::vec3 r0 = contact.position - mBodyA.GetPosition();
			glm::vec3 r1 = contact.position - mBodyB.GetPosition();

			glm::vec3 v0 = mBodyA.GetVelocity() + glm::cross(mBodyA.GetAngularVel(), r0);
			glm::vec3 v1 = mBodyB.GetVelocity() + glm::cross(mBodyB.GetAngularVel(), r1);

			// Relative velocity of B with respect to A. The normal points from A to B,
			// so dot(dv, normal) < 0 means the bodies are approaching.
			glm::vec3 dv = v1 - v0;

			// NORMAL Impulse Code

			// Compute Normal Impulse
			float vn = glm::dot(dv, normal);

			// Works out the bias to prevent Prevents sinking!
			const float allowedPenetration = 0.1f;
			const float biasFactor = 0.1f; // 0.1 to 0.3
			float       biasFactorValue = biasFactor;

			float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
			float bias = biasFactorValue * inv_dt * glm::max(0.0f, contact.depth - allowedPenetration);

			float kNormal = invMass0 + invMass1 +

			                glm::dot(normal,
			                         glm::cross((glm::cross(r0, normal) * mBodyA.I()), r0) +
			                             glm::cross((glm::cross(r1, normal) * mBodyB.I()), r1));

			float massNormal = 1.0f / kNormal;

			float dPn = massNormal * (-vn + bias);

			dPn = glm::max(dPn, 0.0f);

			// Apply normal contact impulse
			glm::vec3 P = dPn * normal;

			mBodyA.ApplyImpulse(-P, r0);
			mBodyB.ApplyImpulse(P, r1);

			// Tangent friction impulse
			{
				glm::vec3 tangent = glm::vec3(0, 0, 0);
				tangent = dv - (glm::dot(dv, normal) * normal);
				if (fabs(glm::length(tangent)) < FLT_EPSILON)
					continue; // no sliding at this point; move on to the next one
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

				mBodyA.ApplyImpulse(-P, r0);
				mBodyB.ApplyImpulse(P, r1);
			}
		}
	}
} // namespace Eklavya::Physics
