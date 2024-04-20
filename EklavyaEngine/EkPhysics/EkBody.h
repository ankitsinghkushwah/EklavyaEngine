#ifndef INC_EKBODY_H
#define INC_EKBODY_H

#include <glm/glm.hpp>
#include <limits>
#include <Components/EkComponent.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "TransformComponent.h"

namespace Eklavya
{
  class EkActor;
  class TransformComponent;
} // namespace Eklavya

namespace Eklavya::Physics
{
  class ICollider;

  enum EBodyType
  {
    STATIC_BODY,
    DYNAMIC_BODY
  };

  class EkBody : public EkComponent
  {
  public:
    EkBody(SHARED_TRANSFORM transform, std::shared_ptr<ICollider> pCollider);
    EkBody(std::shared_ptr<ICollider> pCollider);
    EkBody() = delete;
    ~EkBody();

    void Integrate(float deltaTime);

    void ApplyLinearForce(const glm::vec3 &direction, float mag)
    {
      mF += mMass * direction * mag;
    }

    void ApplyGravityForce()
    {
      if (mMass < FLT_MAX)
        mF += mMass * glm::vec3(0.0f, -9.8 * mGravityScale, 0.0f);
    }

    inline void  SetBodyType(EBodyType type) { mType = type; }

    void         SetMass(float mass);

    inline float GetInverseMass() { return mInvMass; }

    inline float GetMass() const
    {
      if (mInvMass == 0)
        {
          return std::numeric_limits<float>::infinity();
        }
      else
        {
          return ((float)1.0f) / mInvMass;
        }
    }

    inline bool  HasFiniteMass() const { return mInvMass > 0.0f; }

    inline float I() { return mInvI; }

    void         SetPos(const glm::vec3 &pPosition);

    inline void SetOrientation(const glm::quat &rotation) { mTheta = rotation; }

    inline void SetRotation(const glm::vec3 &eulerAngles)
    {
      mTheta = glm::quat(eulerAngles);
    }

    inline void SetAngularVel(glm::vec3 pVel) { mThetaV = pVel; };

    inline void SetRestitution(float value)
    {
      mE = glm::clamp(0.0f, 1.0f, value);
    }

    inline void SetAccel(glm::vec3 pAccel) { mA = pAccel; }

    inline void SetAccel(float x, float y, float z) { mA = glm::vec3(x, y, z); }

    inline float     GetRestitution() { return mE; }

    inline glm::vec3 GetAccel() { return mA; }

    inline void      SetAngularAcc(glm::vec3 pAccel) { mThetaA = pAccel; }

    glm::vec3        GetVelocity() { return this->mV; }

    glm::vec3        GetVelocityAtPoint(const glm::vec3 &point)
    {
      glm::vec3 r = point - mP;

      glm::vec3 Vel = mV + glm::cross(mThetaV, r);
      return Vel;
    }

    inline glm::vec3 GetAngularVel() { return mThetaV; }

    void             ApplyAngularImpulse(const glm::vec3 &force)
    {
      mThetaV += force * mInvI;
    }

    void ClearForces();
    void ClearTorques();

    void Clear()
    {
      mV = glm::vec3(0.0f);
      mThetaV = glm::vec3(0.0f);
      mA = glm::vec3(0.0f);
      mThetaA = glm::vec3(0.0f);
      ClearForces();
      ClearTorques();
    }

    void ApplyImpulseAtPoint(const glm::vec3 &J, const glm::vec3 &point)
    {
      glm::vec3 r = point - mP;
      mV += J * mInvMass;
      mThetaV += glm::cross(r, J) * mInvI;
    }

    void ApplyImpulse(const glm::vec3 &J, const glm::vec3 &r)
    {
      mV += J * mInvMass;
      mThetaV += glm::cross(r, J) * mInvI;
    }

    void             AddAngularVelocity(glm::vec3 value) { mThetaV += value; }

    void             SetVelocity(glm::vec3 vel) { mV = vel; }

    inline glm::vec3 GetPosition() { return mP; }

    inline std::shared_ptr<ICollider> GetCollider() { return mCollider; }

    glm::vec3        GetPointInLocalSpace(const glm::vec3 &point);
    glm::vec3        GetPointInWorldSpace(const glm::vec3 &point);

    glm::mat4        GetWorldMatrix() { return mColliderTR; }

    inline EBodyType GetBodyType() { return mType; }

    void      SetLinearDamping(float damping) { mLinearDamping = damping; }

    void      SetAngularDamping(float damping) { mAngDamping = damping; }

    glm::vec3 GetAxis(int index);

    void      SetFreezePositionFlags(glm::vec3 flags)
    {
      mFreezePositionFlags = flags;
    }

    void SetDisableRotation(bool disable) { mDisableRotation = disable; }

    void UpdateTransform();

    void SetAwake(bool awake)
    {
      mIsAwake = awake;
    }

    bool IsAwake() const { return mIsAwake; }

    void PutOnSleepOnCollision(bool putOnSleep)
    {
      mPutOnSleepOnCollision = putOnSleep;
    }

    bool  ShouldPutOnSleepOnCollision() const { return mPutOnSleepOnCollision; }

    float mI;
    float mInvI;
    float mGravityScale = 1.0f;

#ifdef EKDEBUG
    bool mShowCollider = false;
#endif

  private:
    // forces
    glm::vec3 mTau;
    glm::vec3 mF;

    // object props
    float mInvMass;
    float mMass;
    float mE;

    // linear motion
    glm::vec3 mV;
    glm::vec3 mA;
    glm::vec3 mP;
    float     mLinearDamping;

    // Angular motion

    glm::vec3                  mThetaV;
    glm::vec3                  mThetaA;
    glm::quat                  mTheta;
    float                      mAngDamping;

    std::shared_ptr<ICollider> mCollider;

    glm::mat4                  mColliderTR;
    SHARED_TRANSFORM           mTransform;

    EBodyType                  mType;

    glm::vec3                  mFreezePositionFlags;
    bool                       mDisableRotation = false;

    bool                       mIsAwake = true;

    bool                       mPutOnSleepOnCollision = false;
  };

  DECLARE_COMPONENT_ESSENTIALS(EkBody, EKBODY);
} // namespace Eklavya::Physics

#endif
