#include "EkActor.h"
#include <algorithm>
#include "RenderComponent.hpp"

namespace Eklavya {
EkActorID EkActor::s_IdCounter = 0;

EkActor::EkActor() : mID(++s_IdCounter) {
  mTransform = EmplaceComponent<TransformComponent>();
}

EkActor::~EkActor() {}

SHARED_ACTOR EkActor::GetChild(const std::string &name, bool remove) {

  SHARED_ACTOR foundActor = nullptr;

  auto iter =
      std::find_if(mKids.begin(), mKids.end(), [&](const SHARED_ACTOR &actor) {
        return actor->mName == name;
      });

  if (iter != mKids.end()) {
    foundActor = *iter;
    mKids.erase(iter);
    return foundActor;
  }

  for (SHARED_ACTOR kid : mKids) {
    foundActor = GetChild(name);
    if (foundActor != nullptr) {
      break;
    }
  }

  return foundActor;
}

void EkActor::SetBound(const glm::vec3 &halfExtents) {
  if (SHARED_RENDERCOMPONENT renderComponent =
          GetComponent<RenderComponent>("RenderComponent")) {
    renderComponent->UpdateBounds(-halfExtents, halfExtents);
  }
}

void EkActor::AddKid(SHARED_ACTOR actor) {
  actor->mParent = std::shared_ptr<EkActor>(this);
  mKids.push_back(actor);
}
} // namespace Eklavya
