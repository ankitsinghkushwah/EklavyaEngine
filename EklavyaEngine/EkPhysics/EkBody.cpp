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
#include "ComponentIds.h"

namespace Eklavya::Physics
{

	EkBody::EkBody(EkActor& owner)
	    : EkComponent(owner, CoreComponentIds::RIGIDBODY_COMPONENT_ID)
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
	{
	}

	EkBody::~EkBody()
	{
	}

	const BaseColliderComponent* EkBody::GetCollider() const
	{
		const BaseColliderComponent* collider = GetOwner().GetComponent<BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID);
		return collider;
	}

	void EkBody::UpdateTransform()
	{
		mTheta = glm::normalize(mTheta);

		if (BaseColliderComponent* collider = GetOwner().GetComponent<BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID))
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

		// update position
		glm::vec3 acc = mInvMass * mF;
		mV += acc * delta;
		glm::vec3 angularAcc = 1.0f * mTau;
		mThetaV += angularAcc * delta;
		mV *= pow(mLinearDamping, delta);
		mThetaV *= pow(mAngDamping, delta);
		mP += (mV * delta);

		// update rotation
		glm::quat w(mThetaV * delta);
		w.w = 0.0f;
		w *= mTheta;
		mTheta += (w * 0.5f);

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
			BaseColliderComponent* colliderComponent = GetOwner().GetComponent<BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID);

			glm::vec3 dim;
			if (colliderComponent->GetType() == EColliderType::BOX)
				dim = static_cast<BoxColliderComponent*>(colliderComponent)->GetHalfSize();
			else
			{
				float radii = static_cast<SphereColliderComponent*>(colliderComponent)->GetRadius();
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
