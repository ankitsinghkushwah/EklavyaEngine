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

#define DECLARE_COMPONENT_ESSENTIALS(TYPE, NAME)                               \
  using SHARED_##NAME = std::shared_ptr<TYPE>

#ifdef EKDEBUG
namespace Eklavya::Renderer {
class DebugRenderer;
}
#endif

namespace Eklavya {
using EkComponentID = std::string;

class EkComponent {
public:
  EkComponent(const EkComponentID id);

  virtual void Init() {}

  virtual void FixedTick(float fixedDeltaTime) {}

  virtual void Tick(float deltaTime) {}

  inline const EkComponentID &GetID() { return m_ComponentID; }

#ifdef EKDEBUG
  virtual void DebugDraw(Renderer::DebugRenderer &debugRenderer) {}
#endif

  bool operator==(const EkComponent &rhs) const;
  bool operator==(const EkComponentID &uid) const;

  std::shared_ptr<class EkActor> mOwner;

private:
  EkComponentID m_ComponentID;
};

} // namespace Eklavya

using SHARED_COMPONENT = std::shared_ptr<Eklavya::EkComponent>;

#endif /* EkComponent_hpp */
