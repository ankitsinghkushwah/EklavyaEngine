#include <EkPhysics/EkBody.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Collider.h"
#include <EkPhysics/EkBody.h>
#include <Components/TransformComponent.h>
#include "../Helpers.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <float.h>

#include <EkPhysics/PhysicsWorld.h>
#include <Scene/EkActor.h>
#include <Components/ComponentIds.h>

namespace Eklavya::Physics
{
	EkBody::EkBody(EkActor &owner)
		:
		EkComponent(owner, CoreComponentIds::RIGIDBODY_COMPONENT_ID)
		, mInvMass(0.0f)
		, mMass(FLT_MAX)
		, mF(0.0f, 0.0f, 0.0f)
		, mTau(0.0f, 0.0f, 0.0f)
		,
		// Linear
		mP(0.0f, 0.0f, 0.0f)
		, mV(0.0f, 0.0f, 0.0f)
		, mLinearDamping(.99f)
		,
		// angular
		mThetaA(0.0f)
		, mTheta(glm::vec3(0.0f))
		, mThetaV(0.0f)
		, mAngDamping(.99f)
		,
		// matrices
		mI(FLT_MAX)
		, mInvI(0.0f)
		, mE(0.0f)

		, mFreezePositionFlags(1.0f) {}

	EkBody::~EkBody() {}

	const BaseColliderComponent *EkBody::GetCollider() const
	{
		const BaseColliderComponent *collider = GetOwner().GetComponent<BaseColliderComponent>(
			CoreComponentIds::COLLIDER_COMPONENT_ID);
		return collider;
	}

	void EkBody::UpdateTransform()
	{
		mTheta = glm::normalize(mTheta);

		// Update world inertia tensor
		glm::mat3 R = glm::toMat3(mTheta); // Rotation matrix
		mIWorld = R * mI * glm::transpose(R);
		mInvIWorld = R * mInvI * glm::transpose(R);

		if (BaseColliderComponent *collider = GetOwner().GetComponent<BaseColliderComponent>(
			CoreComponentIds::COLLIDER_COMPONENT_ID))
		{
			collider->UpdateTransform(mP, mTheta);
		}

		GetOwner().Transform().SetPosition(mP);
		GetOwner().Transform().SetRotation(mTheta);
	}

	void EkBody::ClearForces()
	{
		mF = glm::vec3(0.0f);
	}

	void EkBody::ClearTorques()
	{
		mTau = glm::vec3(0.0f);
	}

	void EkBody::Integrate(float delta)
	{
		if (HasFiniteMass() == false)
		{
			UpdateTransform();
			return;
		}

		// Linear motion (unchanged)
		glm::vec3 acc = mInvMass * mF;
		mV += acc * delta;
		mV *= pow(mLinearDamping, delta);
		mP += (mV * delta);

		if (mDisableRotation == false)
		{
			// Angular motion - now using world inertia tensor
			glm::vec3 angularAcc = mInvIWorld * mTau; // Matrix-vector multiply
			mThetaV += angularAcc * delta;
			mThetaV *= pow(mAngDamping, delta);

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
			BaseColliderComponent *colliderComponent = GetOwner().GetComponent<BaseColliderComponent>(
				CoreComponentIds::COLLIDER_COMPONENT_ID);

			if (colliderComponent->GetType() == EColliderType::BOX)
			{
				glm::vec3 halfSize = static_cast<BoxColliderComponent *>(colliderComponent)->GetHalfSize();
				float hx = halfSize.x, hy = halfSize.y, hz = halfSize.z;

				// Box inertia tensor
				mI = glm::mat3(0.0f);
				mI[0][0] = (mass / 12.0f) * (hy * hy + hz * hz); // Ixx
				mI[1][1] = (mass / 12.0f) * (hx * hx + hz * hz); // Iyy
				mI[2][2] = (mass / 12.0f) * (hx * hx + hy * hy); // Izz

				mInvI = glm::inverse(mI);
			}
			else // Sphere
			{
				float r = static_cast<SphereColliderComponent *>(colliderComponent)->GetRadius();
				float I = (2.0f / 5.0f) * mass * r * r;

				mI = glm::mat3(I);
				mInvI = glm::mat3(1.0f / I);
			}

			mInvMass = 1.0f / mass;
		}
		else
		{
			mI = glm::mat3(FLT_MAX);
			mInvI = glm::mat3(0.0f);
			mInvMass = 0.0f;
		}

		// Initialize world inertia (will be updated in UpdateTransform)
		mIWorld = mI;
		mInvIWorld = mInvI;
	}
} // namespace Eklavya::Physics
