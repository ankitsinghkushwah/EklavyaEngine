//
//  CollisionSystem.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#include "CollisionSystem.h"
#include "Collider.h"
#include <EkPhysics/EkBody.h>
#include <Scene/EkActor.h>

#ifdef EKDEBUG
#include <Renderer/DebugRenderer.hpp>
#endif

namespace Eklavya::Physics::CollisionSystem
{
	bool RayVsOBB(Ray ray, const BoxColliderComponent &boxCollider, float &t)
	{
		ray.d = glm::normalize(ray.d);

		float tMin = -FLT_MAX;
		float tMax = FLT_MAX;

		const glm::mat4 &worldMatrix = boxCollider.GetOwner().Transform().GetWorldMatrix();
		const glm::vec3 halfExtents = boxCollider.GetHalfSize();

		glm::vec3 center = worldMatrix[3];

		for (int i = 0; i < 3; i++)
		{
			float t1 = -FLT_MAX;
			float t2 = FLT_MAX;

			glm::vec3 normal = glm::column(worldMatrix, i);
			glm::vec3 delta = center - ray.o;
			float l = glm::length(normal) * 0.5f;
			normal = glm::normalize(normal);

			const float min = -l;
			const float max = l;

			float e = glm::dot(normal, delta);
			float f = glm::dot(ray.d, normal);
			if (glm::abs(f) > std::numeric_limits<float>::epsilon())
			{
				float t1 = (e + min) / f;
				float t2 = (e + max) / f;

				if (t1 > t2)
				{
					std::swap(t1, t2);
				}

				if (t1 > tMin)
				{
					tMin = t1;
				}

				if (t2 < tMax)
				{
					tMax = t2;
				}

				if (tMax < tMin)
				{
					return false;
				}
			} else
			{
				if (-e + min > 0.0f || -e + max < 0.0f)
					return false;
			}
		}

		t = tMin;
		if (t > 0.0001f)
			return true;
		return false;
	}

	bool RayVsSphere(Ray ray, glm::vec3 sphereCenter, float radius, glm::vec2 &points)
	{
		glm::vec3 L = ray.o - sphereCenter;

		float a = glm::dot(ray.d, ray.d);
		float b = 2.0f * glm::dot(ray.d, L);
		float c = glm::dot(L, L) - radius * radius;

		float discr = b * b - 4 * a * c;
		if (discr < 0)
			return false;

		else if (discr == 0)
			points[0] = points[1] = -0.5 * b / a;
		else
		{
			points[0] = -0.5 * (b + sqrt(discr));
			points[1] = -0.5 * (b - sqrt(discr));
		}

		if (points[0] > points[1])
			std::swap(points[0], points[1]);

		return true;
	}
} // namespace Eklavya::Physics::CollisionSystem
