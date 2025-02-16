#ifndef INC_ACTOR_H
#define INC_ACTOR_H

#include "BoundingVolume.h"
#include <Components/EkComponent.hpp>
#include <Components/TransformComponent.h>
#include <CoreUtils/EkUtils.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum ERenderGroup
{
	MIN = 0,
	SKYBOX = MIN,
	SOLID,
	MODEL,
	WATER,
	BOUND_VOLUME,
	OUTLINED,
	CONTACTS,
	MAX
};

namespace Eklavya
{

	using EkActorID = std::uint32_t;
	using UniqueActor = std::unique_ptr<class EkActor>;

	class EkActor
	{
	  public:
		EkActor();
		~EkActor();

		const std::vector<std::unique_ptr<EkComponent>>& Components() { return mComponents; }

		template<typename ComponentType>
		ComponentType* GetComponent(EkComponentID id)
		{
			auto iter = std::find_if(mComponents.begin(), mComponents.end(), [&](const std::unique_ptr<EkComponent>& comp) { return *comp == id; });

			if (iter != mComponents.end())
			{
				return reinterpret_cast<ComponentType*>((*iter).get());
			}
			else
			{
				return nullptr;
			}
		}

		template<typename ComponentType>
		const ComponentType* GetComponent(EkComponentID id) const
		{
			auto iter = std::find_if(mComponents.begin(), mComponents.end(), [&](const std::unique_ptr<EkComponent>& comp) { return *comp == id; });

			if (iter != mComponents.end())
			{
				return reinterpret_cast<const ComponentType*>((*iter).get());
			}
			else
			{
				return nullptr;
			}
		}

		template<typename ComponentType, typename... Args>
		ComponentType* EmplaceComponent(Args&&... args)
		{
			std::unique_ptr<ComponentType> newComponent = std::make_unique<ComponentType>(*this, std::forward<Args>(args)...);
			ComponentType*                 rawComponent = newComponent.get();
			mComponents.emplace_back(std::move(newComponent));
			return rawComponent;
		}

		EkActor* GetChild(const std::string& name);

		const EkActor* Parent() const { return mParent; }

		EkActor* Parent() { return mParent; }

		void SetParent(EkActor* parent) { mParent = parent; }

		void AddKid(UniqueActor& actor);

		const std::vector<UniqueActor>& Kids() const { return mKids; }

		std::vector<UniqueActor>& Kids() { return mKids; }

		bool operator==(EkActorID rhs) { return mID == rhs; }

		EkActorID ID() const { return mID; }

		void SetBound(const glm::vec3& halfExtents);

		const std::string& Name() const { return mName; }

		void SetName(const std::string& name) { mName = name; }

		const TransformComponent& Transform() const { return *mTransform; }

		TransformComponent& Transform() { return *mTransform; }

		bool IsEnabled() const { return mIsEnabled; }

		void SetEnabled(bool isEnabled) { mIsEnabled = isEnabled; }

#ifdef EKDEBUG
		bool mDebugDrawComponents = false;
#endif

	  private:
		std::string                               mName;
		TransformComponent*                       mTransform = nullptr;
		std::vector<std::unique_ptr<EkComponent>> mComponents;
		std::vector<UniqueActor>                  mKids;
		EkActor*                                  mParent = nullptr;
		EkActorID                                 mID;
		bool                                      mIsEnabled = true;
		static EkActorID                          s_IdCounter;
	};

} // namespace Eklavya

#endif
