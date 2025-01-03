//
//  ComponentIds.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 01/01/2025.
//

#ifndef ComponentIds_h
#define ComponentIds_h

#include <cstdint>

namespace CoreComponentIds
{
	constexpr std::uint32_t TRANSFORM_COMPONENT_ID = 0;
	constexpr std::uint32_t RENDER_COMPONENT_ID = 1;
	constexpr std::uint32_t ANIMATION_COMPONENT_ID = 2;
	constexpr std::uint32_t RIGIDBODY_COMPONENT_ID = 3;
	constexpr std::uint32_t COLLIDER_COMPONENT_ID = 4;
  
} // namespace CoreComponentIds

namespace VehiclePhysicsComponentIds
{
  constexpr std::uint32_t SUSPENSION_COMPONENT_ID = 5;
  constexpr std::uint32_t CAR_COMPONENT_ID = 6;
}

namespace ThirdPersonDemoComponentIds
{

}

#endif /* ComponentIds_h */
