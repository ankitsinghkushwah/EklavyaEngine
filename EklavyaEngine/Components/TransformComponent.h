#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "EkComponent.hpp"

namespace Eklavya
{
	class TransformComponent : public EkComponent
	{
	  public:
		TransformComponent(EkActor& owner);
		~TransformComponent();

		void SetRotation(const glm::quat& pRotation);
		void SetRotation(const glm::vec3& pNewYawPitchRoll);

		inline void SetPosition(float x, float y, float z)
		{
			mLocalPosition = glm::vec3(x, y, z);
			UpdateData();
		}

		inline void SetPosition(const glm::vec3& pNewPosition)
		{
			mLocalPosition = pNewPosition;
			UpdateData();
		}

		inline void SetScale(float x, float y, float z)
		{
			mLocalScale = glm::vec3(x, y, z);
			UpdateData();
		}

		inline void SetScale(float uniformScale)
		{
			mLocalScale = glm::vec3(uniformScale);
			UpdateData();
		}

		inline void SetScale(glm::vec3 scale)
		{
			mLocalScale = scale;
			UpdateData();
		}

		void ApplyParentTransform(const glm::mat4& parentT);

		inline const glm::quat& Rotation()
		{
			return mRotation;
		}

		inline const glm::quat& LocalRotation()
		{
			return mLocalRotation;
		}

		inline const glm::vec3& Position() const
		{
			return mPosition;
		}

		inline const glm::vec3& GetScale() const
		{
			return mScale;
		}

		glm::vec3 TransformToLocal(const glm::vec3& vertex);
		glm::vec3 TransformToWorld(const glm::vec3& vertex);

		const glm::mat4& GetWorldMatrix()
		{
			return mWorld;
		}

		glm::mat4 GetRotationMatrix() const;

		const glm::vec3& forward()
		{
			return mForward;
		}

		const glm::vec3& up(bool local = false)
		{
			if (local)
				return mLocalUp;

			return mUp;
		}

		const glm::vec3& right()
		{
			return mRight;
		}

	  private:
		void UpdateBasis();

		void      UpdateData();
		glm::mat4 mParent;
		glm::mat4 mWorld;
		glm::mat4 mLocal;

		glm::vec3 mLocalPosition;
		glm::quat mLocalRotation;
		glm::vec3 mLocalScale;

		glm::vec3 mPosition;
		glm::quat mRotation;
		glm::vec3 mScale;

		glm::vec3 mRight;
		glm::vec3 mUp;
		glm::vec3 mForward;

		glm::vec3 mLocalUp;
		glm::vec3 mLocalRight;
		glm::vec3 mLocalLeft;

		bool mValid = false;
	};

} // namespace Eklavya
