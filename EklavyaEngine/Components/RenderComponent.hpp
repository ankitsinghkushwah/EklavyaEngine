//
//  MeshComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 21/05/2023.
//

#ifndef MeshComponent_hpp
#define MeshComponent_hpp

#include <Components/EkComponent.hpp>
#include <glm/glm.hpp>
#include <string>
#include <Renderer/Material.h>
#include <AssetManager/GLMesh.hpp>
#include <Scene/BoundingVolume.h>
#include <AssetManager/AnimationData.h>

namespace Eklavya
{
	namespace Renderer
	{
		class GLRenderer;
	}

	enum class EMeshType
	{
		SPHERE,
		CUBE,
		ASSET
	};

	class RenderComponent : public EkComponent
	{
	  public:
		RenderComponent(EkActor& owner, const EMeshType builtInMesh);
		RenderComponent(EkActor& owner, ModelID modelId);

		void RegisterRenderer(Renderer::GLRenderer* renderer)
		{
			mRenderer = renderer;
		}

		void Tick(float deltaTime) override;
		void SetMesh(const Asset::GLMesh& mesh);
		void UpdateBounds(glm::vec3 min, glm::vec3 max);

		const Asset::GLMesh& GetMesh() const
		{
			return mMesh;
		}

		Asset::GLMesh& GetMesh()
		{
			return mMesh;
		}

		Renderer::ERenderGroup mRenderGroup = Renderer::ERenderGroup::ACTOR;

		glm::mat4 mWorldMatrix;

		bool mProjectsShadow = true;

		Bound                  mBound;
		bool                   mInsideFrustum = true;
		std::vector<glm::mat4> boneTransforms;
		bool                   mHasBones = false;
		ModelID                mModelID = -1;

#ifdef EKDEBUG
		bool mShowBound = false;
#endif

	  private:
		Renderer::GLRenderer* mRenderer = nullptr;
		Asset::GLMesh         mMesh;
		EMeshType             mMeshType;
	};

} // namespace Eklavya
#endif /* MeshComponent_hpp */
