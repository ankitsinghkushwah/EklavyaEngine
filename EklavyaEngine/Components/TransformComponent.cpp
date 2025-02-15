#include <Components/TransformComponent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <CoreUtils/EkUtils.hpp>
#include <Components/ComponentIds.h>

namespace Eklavya
{

	TransformComponent::TransformComponent(EkActor& owner)
	    : EkComponent(owner, CoreComponentIds::TRANSFORM_COMPONENT_ID)
	    , mRotation()
	    , mScale(1.0f)
	    , mPosition(0.0f)
	    , mLocalPosition(0.0f)
	    , mLocalScale(1.0f)
	    , mLocalRotation()
	    , mWorld(1.0f)
	    , mLocal(1.0f)
	    , mParent(1.0f)
	    ,

	    mForward(0.0f, 0.0f, -1.0f)
	    , mRight(1.0f, 0.0f, 0.0f)
	    , mUp(0.0f, 1.0f, 0.0f)

	{
	}

	TransformComponent::~TransformComponent()
	{
	}

	void TransformComponent::SetRotation(const glm::vec3& pEulerAngles)
	{
		glm::vec3 rads = {glm::radians(pEulerAngles.x), glm::radians(pEulerAngles.y), glm::radians(pEulerAngles.z)};
		mLocalRotation = glm::quat(rads);
		UpdateData();
	}

	void TransformComponent::SetRotation(const glm::quat& pRotation)
	{
		mLocalRotation = pRotation;
		UpdateData();
	}

	void TransformComponent::UpdateBasis()
	{
		mRight = glm::normalize(glm::vec3(mWorld[0][0], mWorld[0][1], mWorld[0][2]));
		mUp = glm::normalize(glm::vec3(mWorld[1][0], mWorld[1][1], mWorld[1][2]));
		mForward = glm::normalize(glm::vec3(mWorld[2][0], mWorld[2][1], mWorld[2][2]));
	}

	glm::vec3 TransformComponent::TransformToLocal(const glm::vec3& vertex)
	{
		return glm::vec3(glm::inverse(mWorld) * glm::vec4(vertex, 1.0f));
	}

	void TransformComponent::ApplyParentTransform(const glm::mat4& parentT)
	{
		mParent = parentT;
		UpdateData();
	}

	void TransformComponent::UpdateData()
	{
		glm::mat4 rotation = glm::toMat4(mLocalRotation);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), mLocalScale);
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), mLocalPosition);
		mLocal = translation * rotation * scale;
		mWorld = mParent * mLocal;

		glm::vec3 dummy;
		glm::vec4 dummy2;

		glm::decompose(mWorld, mScale, mRotation, mPosition, dummy, dummy2);

		UpdateBasis();
	}

	glm::mat4 TransformComponent::GetRotationMatrix() const
	{
		return glm::toMat4(mRotation);
	}
} // namespace Eklavya
