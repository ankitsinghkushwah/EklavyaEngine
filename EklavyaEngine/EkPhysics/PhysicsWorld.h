#ifndef INC_PHYSICS_WORLD_H
#define INC_PHYSICS_WORLD_H

#include "CollisionSystem.h"
#include "Constraints/ContactConstraint.h"
#include <bitset>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>
#ifdef EKDEBUG
#include <Renderer/DebugRenderer.hpp>
#endif

// Forward Declarations

class IEventData;

namespace Eklavya::Physics
{
	class BaseColliderComponent;
	class BoxColliderComponent;
	class SphereColliderComponent;
	class EkBody;

	class World
	{
	  public:
		World();
		~World();
		void AddBody(EkBody* body);
		void RemoveBody(EkBody* body);
		void Step(float delta);

		CastHitResult RayCast(glm::vec3 o, glm::vec3 direction, float range, int ignoreGroupFlag) const;

#ifdef EKDEBUG
		std::vector<ContactConstraint> mCachedContactsForDebug;
		Renderer::DebugRenderer*       mDebugRenderer = nullptr;
		void                           OnDebugDraw(Renderer::DebugRenderer& debugRenderer);
		void                           ImGuiProc();
#endif
	  private:
		std::vector<EkBody*> mBodies;
		int                  mIterations = 100;
	};
} // namespace Eklavya::Physics

#endif
