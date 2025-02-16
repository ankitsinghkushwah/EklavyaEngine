#ifndef INC_COLLIDER_H_
#define INC_COLLIDER_H_

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <Components/EkComponent.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklavya::Physics {

enum class EColliderType { BOX = 0, SPHERE, MAX };

class BaseColliderComponent : public EkComponent {
public:
  BaseColliderComponent(EkActor &owner, EColliderType type);
  virtual ~BaseColliderComponent();

  EColliderType GetType() const { return mType; }

  const glm::vec3 &GetOffset() const { return mOffset; }

  void SetOffset(const glm::vec3 &offset) { mOffset = offset; }

  void SetGroupIndex(int groupIndex) { mGroupFlag = groupIndex; }

  int GroupFlag() const { return mGroupFlag; }

  void UpdateTransform(glm::vec3 pos, glm::quat rotation);

  glm::vec3 GetPosition() const { return GetAxis(3); }

  glm::mat4 GetWorldMatrix() const { return mColliderTR; }

  glm::vec3 GetPointInLocalSpace(const glm::vec3 &point) const {
    return glm::inverse(mColliderTR) * glm::vec4(point, 1.0f);
  }

  glm::vec3 GetPointInWorldSpace(const glm::vec3 &point) const {
    return glm::vec3(mColliderTR * glm::vec4(point, 1.0f));
  }

  glm::mat3 GetRotationMatrix() const {
    glm::mat3 rotation;
    rotation[0] = glm::vec3(mColliderTR[0]); // First column of the 3x3 rotation
    rotation[1] =
        glm::vec3(mColliderTR[1]); // Second column of the 3x3 rotation
    rotation[2] =
        glm::vec3(mColliderTR[2]); // Third column of the 3x3 rotation}
    return rotation;
  }

  glm::vec3 GetAxis(int index) const { return glm::vec3(mColliderTR[index]); }

protected:
  int mGroupFlag;
  glm::vec3 mOffset;
  EColliderType mType;
  glm::mat4 mColliderTR;
};

class SphereColliderComponent : public BaseColliderComponent {
public:
  SphereColliderComponent(EkActor &owner);
  ~SphereColliderComponent();

  float GetRadius() const { return mRadius; }

  void SetRadius(float radius);

private:
  float mRadius = 0.0f;
};

class BoxColliderComponent : public BaseColliderComponent {
public:
  BoxColliderComponent(EkActor &owner);
  ~BoxColliderComponent();

  glm::vec3 GetHalfSize() const { return mHalfSize; }

  void SetHalfSize(glm::vec3 halfSize);

private:
  glm::vec3 mHalfSize;
};

} // namespace Eklavya::Physics
#endif
