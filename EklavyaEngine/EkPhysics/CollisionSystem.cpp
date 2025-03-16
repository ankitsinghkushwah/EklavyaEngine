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
		const glm::mat4 invWorldMatrix = glm::transpose(worldMatrix);

		glm::vec3 center = worldMatrix[3];

		for (int i = 0; i < 3; i++)
		{
			float t1 = -FLT_MAX;
			float t2 = FLT_MAX;

			glm::vec3 normal = glm::column(worldMatrix, i);
			float l = glm::length(normal) * 0.5f;
			normal = glm::normalize(normal);

			Plane nearPlane;
			nearPlane.o = center + (normal * l);
			nearPlane.n = normal;

			Plane farPlane;
			farPlane.o = center - (normal * l);
			farPlane.n = normal;

#ifdef EKDEBUG
			// float rad = 3.0f;
			// glm::vec4 col(normal, 1.0f);
			// float nLen = 10.0f;
			// Renderer::DebugRenderer::GetInstance().AddSphere(nearPlane.o, rad, col);
			// Renderer::DebugRenderer::GetInstance().AddLine(nearPlane.o, nearPlane.o + normal * nLen, col, .3f);
			// Renderer::DebugRenderer::GetInstance().AddSphere(farPlane.o, rad, col);
			// Renderer::DebugRenderer::GetInstance().AddLine(farPlane.o, farPlane.o + normal * nLen, col, .3f);

#endif

			if (RayVsPlane(ray, nearPlane, t1) == false || RayVsPlane(ray, farPlane, t2) == false)
			{
				continue;
			}


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
