#ifndef INC_FRUSTUM
#define INC_FRUSTUM

#include <glm/glm.hpp>

namespace Eklavya
{
	class VertexArrayObject;
	class IBoundVolume;
	class Bound;

	enum EPlane
	{
		TOP_PLANE = 0,
		BOTTOM_PLANE,
		LEFT_PLANE,
		RIGHT_PLANE,
		FAR_PLANE,
		NEAR_PLANE
	};

	enum EPoint
	{
		NEAR_TOP_LEFT,
		NEAR_TOP_RIGHT,
		NEAR_BOTTOM_RIGHT,
		NEAR_BOTTOM_LEFT,
		FAR_TOP_LEFT,
		FAR_TOP_RIGHT,
		FAR_BOTTOM_RIGHT,
		FAR_BOTTOM_LEFT,
		NEAR_CENTER,
		FAR_CENTER
	};

	using Plane = glm::vec4;

	class Frustum
	{
	  public:
		Frustum()
		{
		}

		void Init(float fov, float ratio, float fNear, float fFar);
		~Frustum();
		void UpdatePlanes(const glm::mat4& projection, const glm::mat4& view);
		void Destroy();

		bool Test(const glm::mat4& transform, const Bound& box) const;

	  public:
		const glm::vec3* Points() const
		{
			return mWorldPoints;
		}

		void OnDebugUpdate(const glm::mat4& view);

	  private:
		void      UpdatePoints();
		glm::vec3 mPoints[10];
		glm::vec3 mWorldPoints[10];

	  private:
		Plane mPlanes[6];

		int       mIndices[36];
		float     mNearWidth, mNearHeight, mFarWidth, mFarHeight;
		glm::mat4 mModel;
	};

} // namespace Eklavya
#endif
