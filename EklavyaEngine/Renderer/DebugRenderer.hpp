//
//  DebugHelper.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 25/06/2023.
//
#ifndef DebugHelper_hpp
#define DebugHelper_hpp

#include <AssetManager/Asset.h>
#include <AssetManager/VertexArrayObject.h>
#include <Scene/BoundingVolume.h>
#include <glm/glm.hpp>
#include <list>
#include <vector>

namespace Eklavya
{
	class TransformComponent;
	class Frustum;
} // namespace Eklavya

namespace Eklavya::Renderer
{
	namespace
	{
		struct DebugLine
		{
			glm::vec3 start, end;
			glm::vec4 color;
			float     thickness;

			DebugLine(glm::vec3 s, glm::vec3 e, glm::vec4 c, float t) : start(s), end(e), color(c), thickness(t) {}
		};

		struct DebugSphere
		{
			glm::vec3 center;
			glm::vec4 color;
			float     radius;

			DebugSphere(glm::vec3 p, glm::vec4 c, float r) : center(p), color(c), radius(r) {}
		};
	} // namespace

	class DebugRenderer
	{
	  public:
		DebugRenderer();

		~DebugRenderer();

		void SetData(glm::mat4& projection, glm::mat4& view);

		void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness, bool disableDepthTest = false);

		void DrawSphere(glm::vec3 center, float radius, glm::vec4 color);

		void DrawSphere(glm::vec3 center, glm::vec3 extents, glm::vec4 color);

		void DrawBox(glm::vec3 center, glm::vec3 rotation, glm::vec3 extents, glm::vec4 color);

		void DrawBound(const glm::mat4& model, const Bound& bound, glm::vec4 color);

		void DrawPoints(std::vector<glm::vec3>& points, glm::vec4 color);

		void DrawTransform(TransformComponent& transform);

		void DrawFrustum(const Frustum& frustum);

		void AddLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness);

		void AddSphere(glm::vec3 center, float radius, glm::vec4 color);

		void DrawAddedShapes();

	  private:
		glm::mat4 GetModel(glm::vec3 t, const glm::quat& r, glm::vec3 s);

		glm::mat4     mProjection;
		glm::mat4     mView;
		SHARED_SHADER mUnlitSolids = nullptr;
		SHARED_SHADER mWorldPoints = nullptr;

		VertexArrayObject mSphereVAO;
		VertexArrayObject mBoxVAO;

		std::list<DebugLine>   mLinesToDraw;
		std::list<DebugSphere> mSpheresToDraw;
	};
} // namespace Eklavya::Renderer

#endif /* DebugHelper_hpp */
