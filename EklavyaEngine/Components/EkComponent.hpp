//
//  EkComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 20/05/2023.
//

#ifndef EkComponent_hpp
#define EkComponent_hpp

#include <cstdint>
#include <memory>
#include <array>
#include <string>
#include <Components/ComponentIds.h>

#ifdef EKDEBUG
namespace Eklavya::Renderer
{
	class DebugRenderer;
}
#endif

namespace Eklavya
{
	class EkActor;

	using EkComponentID = std::uint32_t;

	class EkComponent
	{
	  public:
		EkComponent(EkActor& owner, const EkComponentID id);

		virtual void FixedTick(float fixedDeltaTime)
		{
		}

		virtual void Tick(float deltaTime)
		{
		}

		const EkActor& GetOwner() const
		{
			return mOwner;
		}
		EkActor& GetOwner()
		{
			return mOwner;
		}

#ifdef EKDEBUG
		virtual void DebugDraw(Renderer::DebugRenderer& debugRenderer)
		{
		}
#endif

		bool operator==(const EkComponent& rhs) const;
		bool operator==(const EkComponentID& uid) const;

	  private:
		EkActor&      mOwner;
		EkComponentID mComponentID;
	};

} // namespace Eklavya

#endif /* EkComponent_hpp */
