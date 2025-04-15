#include <EkPhysics/PhysicsWorld.h>
#include "Collider.h"
#include <EkPhysics/EkBody.h>
#include <algorithm>
#include <Scene/EkActor.h>
#include <Components/TransformComponent.h>
#include "imgui/imgui.h"
#include "ContactGenerator.h"
#include <EkPhysics/Constraints/Constraint.h>

#include "glm/gtc/matrix_access.hpp"

namespace Eklavya::Physics
{
	World::World() {}

	World::~World() {}

	void World::AddBody(EkBody *body)
	{
		mBodies.push_back(body);
	}

	void World::RemoveBody(EkBody *body)
	{
		auto iter = std::find(mBodies.begin(), mBodies.end(), body);
		if (iter != mBodies.end())
		{
			mBodies.erase(iter);
		}
	}

	void World::Step(float delta)
	{
		for (const auto &body: mBodies)
		{
			body->ApplyGravityForce();
		}

		for (auto *body: mBodies)
			body->Integrate(delta);

		// Generate Contacts
		std::vector<ContactConstraint> constraints = ContactGenerator::CreateConstraints(mBodies);

		for (int i = 0; i < mIterations; i++)
		{
			for (auto &constraint: constraints)
			{
				constraint.Solve(delta);
			}
		}

#ifdef EKDEBUG
		mCachedContactsForDebug = std::move(constraints);
#endif
	}

	CastHitResult World::RayCast(glm::vec3 o, glm::vec3 d, float maxRange, int ignoreGroupFlag) const
	{
		Ray ray{o, d, maxRange};
		CastHitResult result;

		float t = 0.0f;
		int index = -1;
		float shortestT = maxRange;


		for (int i = 0; i < mBodies.size(); i++)
		{
			auto collider = mBodies[i]->GetCollider();

			if (collider->GroupFlag() == ignoreGroupFlag)
				continue;

			bool success = false;

			if (collider->GetType() == EColliderType::BOX)
			{
				const BoxColliderComponent *boxCollider = static_cast<const BoxColliderComponent *>(mBodies[i]->
					GetCollider());

				success = CollisionSystem::RayVsOBB(ray, *boxCollider, t);
			}
			else if (collider->GetType() == EColliderType::SPHERE)
			{
				const SphereColliderComponent *sphereCollider = static_cast<const SphereColliderComponent *>(mBodies[i]
					->GetCollider());
				glm::vec2 points;
				success = CollisionSystem::RayVsSphere(ray, sphereCollider->GetPosition(), sphereCollider->GetRadius(),
				                                       points);
				if (success)
				{
					t = points[0];
				}
			}

			if (success && t < shortestT)
			{
				shortestT = t;
				index = i;
			}
		}

		if (index != -1)
		{
			result.t = shortestT;
			result.success = true;
			result.position = o + d * shortestT;
			result.body = mBodies[index];

			result.normal = glm::normalize(glm::column(result.body->GetOwner().Transform().GetWorldMatrix(), 1));
		}
		else
		{
			result.position = o + d * maxRange;
		}

		return result;
	}

	CastHitResult World::SphereCast(glm::vec3 o, glm::vec3 direction, float radius, float range,
	                                int ignoreGroupFlag) const
	{
		direction = glm::normalize(direction);
		CastHitResult result = RayCast(o, direction, range, ignoreGroupFlag);


		if (result.success)
		{
			//Renderer::DebugRenderer::GetInstance().ClearAddedShapes();
			glm::vec3 sphereCenter = result.position - (direction * radius);
			ContactGenerator::GetNearestContactToSphere(mBodies, sphereCenter, radius, result.position);
			//Renderer::DebugRenderer::GetInstance().AddSphere(sphereCenter, radius, glm::vec4(.5f, 3.0f, 1.0f, .4f));
		}

		return result;
	}

#ifdef EKDEBUG
	void World::OnDebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		if (mDebugRenderer == nullptr)
			mDebugRenderer = &debugRenderer;
		for (auto &constraint: mCachedContactsForDebug)
		{
			for (auto &contact: constraint.GetCollisionPoints())
			{
				glm::vec3 endPoint = contact.location - contact.normal * 10.0f;
				debugRenderer.DrawLine(contact.location, endPoint, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), .2f, true);
				debugRenderer.DrawBox(endPoint, glm::vec3(0.0f), glm::vec3(0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}

	void World::ImGuiProc() {}

#endif
} // namespace Eklavya::Physics
