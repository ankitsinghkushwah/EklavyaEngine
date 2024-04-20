#include "EkBody.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Collider.h"
#include "EkBody.h"
#include <Components/TransformComponent.h>
#include "../Helpers.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <float.h>

#include "PhysicsWorld.h"
#include "EkActor.h"

namespace Eklavya::Physics
{

  EkBody::EkBody(SHARED_TRANSFORM           transform,
                 std::shared_ptr<ICollider> pCollider)
      : EkComponent("EkBody"), mTransform(transform), mCollider(pCollider),
        mInvMass(0.0f), mMass(FLT_MAX), mF(0.0f, 0.0f, 0.0f),
        mTau(0.0f, 0.0f, 0.0f),
        // Linear
        mP(0.0f, 0.0f, 0.0f), mV(0.0f, 0.0f, 0.0f), mA(0.0f, 0.0f, 0.0f),
        mLinearDamping(.99f),
        // angular
        mThetaA(0.0f), mTheta(glm::vec3(0.0f)), mThetaV(0.0f),
        mAngDamping(.99f),
        // matrices
        mI(FLT_MAX), mInvI(0.0f), mColliderTR(1.0f),

        mE(0.0f), // no bounce by default
        mType(DYNAMIC_BODY),

        mFreezePositionFlags(1.0f), mDisableRotation(false)

  {
  }

  EkBody::EkBody(std::shared_ptr<ICollider> pCollider)
      : EkComponent("EkBody"), mTransform(nullptr), mCollider(pCollider),
        mInvMass(0.0f), mMass(FLT_MAX), mF(0.0f, 0.0f, 0.0f),
        mTau(0.0f, 0.0f, 0.0f),
        // Linear
        mP(0.0f, 0.0f, 0.0f), mV(0.0f, 0.0f, 0.0f), mA(0.0f, 0.0f, 0.0f),
        mLinearDamping(.99f),
        // angular
        mThetaA(0.0f), mTheta(glm::vec3(0.0f)), mThetaV(0.0f),
        mAngDamping(.99f),
        // matrices
        mI(FLT_MAX), mInvI(0.0f), mColliderTR(1.0f),

        mE(0.0f), // no bounce by default
        mType(DYNAMIC_BODY),

        mFreezePositionFlags(1.0f), mDisableRotation(false)

  {
  }

  EkBody::~EkBody() {}

  void EkBody::UpdateTransform()
  {
    mTheta = glm::normalize(mTheta);

    glm::mat3 rotation = glm::toMat3(mTheta);

    glm::mat4 translation =
        glm::translate(glm::mat4(1.0f), mP + mCollider->GetOffset());
    mColliderTR = translation * glm::mat4(rotation);

    if (mTransform)
      {
        mTransform->SetPosition(mP);
        mTransform->SetRotation(mTheta);
      }
  }

  glm::vec3 EkBody::GetPointInLocalSpace(const glm::vec3 &point)
  {
    return glm::inverse(mColliderTR) * glm::vec4(point, 1.0f);
  }

  glm::vec3 EkBody::GetPointInWorldSpace(const glm::vec3 &point)
  {
    return glm::vec3(mColliderTR * glm::vec4(point, 1.0f));
  }

  glm::vec3 EkBody::GetAxis(int index) { return glm::vec3(mColliderTR[index]); }

  void      EkBody::SetPos(const glm::vec3 &pPosition) { mP = pPosition; }

  void      EkBody::ClearForces() { mF = glm::vec3(0.0f); }

  void      EkBody::ClearTorques() { mTau = glm::vec3(0.0f); }

  void      EkBody::Integrate(float delta)
  {
    if (!(gDebugFeaturesEnabled & INTEGRATION) || mIsAwake == false)
      {
        UpdateTransform();
        return;
      }

    glm::vec3 acc = mA;
    acc += mInvMass * mF;
    mV += acc * delta;

    glm::vec3 angularAcc = 1.0f * mTau;
    mThetaV += angularAcc * delta;
    mV *= pow(mLinearDamping, delta);
    mThetaV *= pow(mAngDamping, delta);
    mP += (mV * delta) * mFreezePositionFlags;

    if (!mDisableRotation)
      {
        glm::quat w(mThetaV * delta);
        w.w = 0.0f;
        w *= mTheta;
        mTheta += (w * 0.5f);
      }

    UpdateTransform();

    mF = glm::vec3(0.0f);
    mTau = glm::vec3(0.0f);
  }

  void EkBody::SetMass(float mass)
  {
    assert(static_cast<int>(mass) != 0);

    mMass = mass;

    if (mass < FLT_MAX)
      {
        glm::vec3 dim;
        if (mCollider->GetType() == EColliderType::BOX)
          dim = std::static_pointer_cast<BoxCollider>(mCollider)->GetHalfSize();
        else
          {
            float radii = std::static_pointer_cast<SphereCollider>(mCollider)
                              ->GetRadius();
            dim = glm::vec3(radii);
          }

        mInvMass = ((float)1.0f) / mass;
        mI = mass * (dim.x * dim.x + dim.y * dim.y + dim.z * dim.z) / 12.0f;
        mInvI = 1.0f / mI;
      }
    else
      {
        mInvMass = 0.0f;
        mI = FLT_MAX;
        mInvI = 0.0f;
      }
  }
} // namespace Eklavya::Physics
