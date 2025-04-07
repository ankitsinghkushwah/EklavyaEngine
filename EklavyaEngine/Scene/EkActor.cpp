#include <Scene/EkActor.h>
#include <algorithm>
#include <Components/RenderComponent.hpp>

namespace Eklavya
{
	EkActorID EkActor::s_IdCounter = 0;

	EkActor::EkActor() :
		mID(++s_IdCounter)
	{
		mTransform = EmplaceComponent<TransformComponent>();
	}

	EkActor::~EkActor() {}

	EkActor *EkActor::GetChild(const std::string &name)
	{
		EkActor *foundActor = nullptr;

		auto iter = std::find_if(mKids.begin(), mKids.end(), [&](const UniqueActor &actor)
		{
			return actor->mName == name;
		});

		if (iter != mKids.end())
		{
			foundActor = (*iter).get();
			mKids.erase(iter);
			return foundActor;
		}

		for (const UniqueActor &kid: mKids)
		{
			foundActor = kid->GetChild(name);
			if (foundActor != nullptr)
			{
				break;
			}
		}

		return foundActor;
	}

	void EkActor::SetBound(const glm::vec3 &halfExtents)
	{
		if (RenderComponent *renderComponent = GetComponent<RenderComponent>(CoreComponentIds::RENDER_COMPONENT_ID))
		{
			renderComponent->UpdateBounds(-halfExtents, halfExtents);
		}
	}

	void EkActor::AddKid(UniqueActor &kid)
	{
		kid->SetParent(this);
		mKids.push_back(std::move(kid));
	}
} // namespace Eklavya
