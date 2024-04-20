#ifndef INC_COMPONENT_H
#define INC_COMPONENT_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <Components/EkComponent.hpp>
#include "EkUtils.hpp"
#include <utility>
#include "TransformComponent.h"
#include "BoundingVolume.h"

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

  using SHARED_ACTOR = std::shared_ptr<class EkActor>;

  // forward decl end
  using EkActorID = unsigned int;

  class EkActor
  {
  public:
    EkActor();
    ~EkActor();

    std::vector<SHARED_COMPONENT> &Components() { return mComponents; }

    template <typename ComponentType>
    std::shared_ptr<ComponentType> GetComponent(const EkComponentID id)
    {
      auto iter = std::find_if(
          mComponents.begin(), mComponents.end(),
          [&](const SHARED_COMPONENT comp) { return *comp == id; });

      if (iter != mComponents.end())
        {
          return std::static_pointer_cast<ComponentType>(*iter);
        }
      else
        return nullptr;
    }

    template <typename ComponentType, typename... Args>
    std::shared_ptr<ComponentType> EmplaceComponent(Args &&...args)
    {
      std::shared_ptr<ComponentType> newComponent =
          std::make_shared<ComponentType>(std::forward<Args>(args)...);
      mComponents.emplace_back(newComponent);
      newComponent->mOwner = std::shared_ptr<EkActor>(this);
      newComponent->Init();
      return newComponent;
    }

    SHARED_ACTOR GetChild(const std::string &name, bool remove = false);

    SHARED_ACTOR Parent() const { return mParent; }

    void         SetParent(const SHARED_ACTOR parent) { mParent = parent; }

    void         AddKid(SHARED_ACTOR actor);

    const std::vector<SHARED_ACTOR> &Kids() const { return mKids; }

    std::vector<SHARED_ACTOR>       &Kids() { return mKids; }

    bool                    operator==(EkActorID rhs) { return mID == rhs; }

    EkActorID               ID() const { return mID; }

    void                    SetBound(const glm::vec3 &halfExtents);

    const std::string      &Name() const { return mName; }

    void                    SetName(const std::string &name) { mName = name; }

    const SHARED_TRANSFORM &Transform() const { return mTransform; }

    SHARED_TRANSFORM        Transform() { return mTransform; }

    bool                    IsEnabled() const { return mIsEnabled; }

    void SetEnabled(bool isEnabled) { mIsEnabled = isEnabled; }

#ifdef EKDEBUG
    bool mDebugDrawComponents = false;
#endif

  private:
    std::string                   mName;
    SHARED_TRANSFORM              mTransform = nullptr;
    std::vector<SHARED_COMPONENT> mComponents;
    std::vector<SHARED_ACTOR>     mKids;
    SHARED_ACTOR                  mParent = nullptr;
    EkActorID                     mID;
    bool                          mIsEnabled = true;
    static EkActorID              s_IdCounter;
  };
} // namespace Eklavya

#endif
