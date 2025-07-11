//
//  ICamera.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 31/03/2024.
//

#ifndef ICamera_hpp
#define ICamera_hpp

#include <glm/glm.hpp>
#include "Frustum.h"
#include <UserInputListener.h>

#include "Renderer/GLRenderer.h"

#include <Renderer/DebugRenderer.hpp>


namespace Eklavya
{
	struct CameraParams
	{
		float fov = 45.0f;
		float ratio = 1.0f;
		float near = 0.1f;
		float far = 100.0f;
	};

	class ICamera : public UserInputListener
	{
	public:
		ICamera(CameraParams options);

		ICamera();

		virtual void Update(float deltaTime);

		glm::mat4 GetView() const
		{
			return mView;
		}

		glm::mat4 GetProjection() const
		{
			return mProjection;
		}

		glm::mat4 GetClipMatrix() const
		{
			return mProjection * mView;
		}

		const Frustum &GetFrustum() const
		{
			return mFrustum;
		}

		glm::vec3 Forward() const
		{
			return mForward;
		}

		glm::vec3 Position() const
		{
			return mPosition;
		}

		//inputs
		void OnKeyAction(int key, int action) override;

		void SetEnabled(bool enabled) { mEnabled = enabled; }

	protected:
		glm::mat4 mView;
		glm::mat4 mProjection;
		Frustum mFrustum;
		glm::vec3 mForward;
		glm::vec3 mPosition;

		bool mEnabled = false;

	private:
		bool mDebug = false;

	public:
		virtual void DebugDraw(Renderer::DebugRenderer &debugRenderer);
	};
} // namespace Eklavya

#endif /* ICamera_hpp */
