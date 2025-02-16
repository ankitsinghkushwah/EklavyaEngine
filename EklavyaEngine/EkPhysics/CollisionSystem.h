//
//  CollisionSystem.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef CollisionSystem_hpp
#define CollisionSystem_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace Eklavya::Renderer
{
	class DebugRenderer;
}

namespace Eklavya::Physics
{

	class EkBody;
	class BoxColliderComponent;
	class SpheereColliderComponent;

	enum EPhysicsGroupFlag
	{
		STATIC,
		OTHER_COLLIDABLES
	};

	struct Ray
	{
		glm::vec3 o;
		glm::vec3 d;
		float     range;

		constexpr glm::vec3 GetPoint(float t) const
		{
			return o + d * t;
		}
	};

	struct Plane
	{
		glm::vec3 o;
		glm::vec3 n;
	};

	struct CastHitResult

	{
		float     t;
		EkBody*   body = nullptr;
		glm::vec3 normal;
		glm::vec3 position;
		bool      success = false;

		CastHitResult() : t(0.0f), normal(0.0f), position(0.0f)
		{
		}
	};
	namespace CollisionSystem
	{
		inline bool RayVsPlane(Ray ray, Plane plane, float& t)
		{
			float denom = glm::dot(plane.n, ray.d);
			// ray is parallel to plan?
			if (glm::abs(denom) > 0.0f)
			{
				float num = glm::dot(plane.n, (plane.o - ray.o));
				t = num / denom;
				if (t > 0.0f)
					return true;
			}
			return false;
		}

#ifdef EKDEBUG
		bool RayVsOBB(Ray ray, const BoxColliderComponent& boxCollider, float& t, Eklavya::Renderer::DebugRenderer&);
#else
		bool RayVsOBB(Ray ray, const BoxColliderComponent& boxCollider, float& t);
#endif

		bool RayVsSphere(Ray ray, glm::vec3 sphereCenter, float radius, glm::vec2& points);

	} // namespace CollisionSystem
} // namespace Eklavya::Physics

#endif /* CollisionSystem_hpp */
