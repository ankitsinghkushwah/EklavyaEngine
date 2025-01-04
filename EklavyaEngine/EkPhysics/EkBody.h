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
	class BaseColliderComponent;

	class EkBody : public EkComponent
	{
	  public:
		EkBody(EkActor& owner);
		~EkBody();

		void Integrate(float deltaTime);

		void ApplyForce(const glm::vec3& direction, float mag)
		{
			mF += mMass * direction * mag;
		}

		void SetMass(float mass);

		float GetInverseMass()
		{
			return mInvMass;
		}

		float GetMass() const
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

		bool HasFiniteMass() const
		{
			return mInvMass > 0.0f;
		}

		float I()
		{
			return mInvI;
		}

		void SetPosition(const glm::vec3& pPosition)
		{
			mP = pPosition;
		}

		void SetOrientation(const glm::quat& rotation)
		{
			mTheta = rotation;
		}

		void SetRotation(const glm::vec3& eulerAngles)
		{
			mTheta = glm::quat(eulerAngles);
		}

		void SetAngularVel(glm::vec3 pVel)
		{
			mThetaV = pVel;
		};

		void SetRestitution(float value)
		{
			mE = glm::clamp(0.0f, 1.0f, value);
		}

		float GetRestitution()
		{
			return mE;
		}

		void SetAngularAcc(glm::vec3 pAccel)
		{
			mThetaA = pAccel;
		}

		glm::vec3 GetVelocity()
		{
			return this->mV;
		}

		glm::vec3 GetVelocityAtPoint(const glm::vec3& point)
		{
			glm::vec3 r = point - mP;

			glm::vec3 Vel = mV + glm::cross(mThetaV, r);
			return Vel;
		}

		glm::vec3 GetAngularVel()
		{
			return mThetaV;
		}

		void ApplyAngularImpulse(const glm::vec3& force)
		{
			mThetaV += force * mInvI;
		}

		void ClearForces();
		void ClearTorques();

		void Clear()
		{
			mV = glm::vec3(0.0f);
			mThetaV = glm::vec3(0.0f);
			mThetaA = glm::vec3(0.0f);
			ClearForces();
			ClearTorques();
		}

		void ApplyImpulseAtPoint(const glm::vec3& J, const glm::vec3& point)
		{
			glm::vec3 r = point - mP;
			mV += J * mInvMass;
			mThetaV += glm::cross(r, J) * mInvI;
		}

		void ApplyImpulse(const glm::vec3& J, const glm::vec3& r)
		{
			mV += J * mInvMass;
			mThetaV += glm::cross(r, J) * mInvI;
		}

		void AddAngularVelocity(glm::vec3 value)
		{
			mThetaV += value;
		}

		void SetVelocity(glm::vec3 vel)
		{
			mV = vel;
		}

		glm::vec3 GetPosition()
		{
			return mP;
		}

		void SetLinearDamping(float damping)
		{
			mLinearDamping = damping;
		}

		void SetAngularDamping(float damping)
		{
			mAngDamping = damping;
		}

		const BaseColliderComponent* GetCollider() const;

		void UpdateTransform();

		void ApplyGravityForce()
		{
			if (mMass < FLT_MAX)
			{
				mF += mMass * glm::vec3(0.0f, -98.0f * mGravityScale, 0.0f);
			}
		}
		void SetFreezePositionFlags(glm::vec3 flags)
		{
			mFreezePositionFlags = flags;
		}

		void SetGravityScale(float value)
		{
			mGravityScale = value;
		}
		void SetDisableRotation(bool disable)
		{
			mDisableRotation = disable;
		}
#ifdef EKDEBUG
		bool mShowCollider = false;
#endif

	  private:
		// forces
		glm::vec3 mTau;
		glm::vec3 mF;

		// object props
		float mInvMass = 1.0f;
		float mMass = 1.0f;
		float mE = 0.0f;
		float mI = 0.0f;
		float mInvI = 0.0f;
		float mGravityScale = 1.0f;

		// linear motion
		glm::vec3 mV;
		glm::vec3 mP;
		float     mLinearDamping = 0.0f;

		// Angular motion

		glm::vec3 mThetaV;
		glm::vec3 mThetaA;
		glm::quat mTheta;
		float     mAngDamping = 0.0f;

		glm::vec3 mFreezePositionFlags;
		bool      mDisableRotation = false;
	};

} // namespace Eklavya::Physics

#endif
