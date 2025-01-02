//
//  MeshComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 21/05/2023.
//
#include "RenderComponent.hpp"
#include "EkComponent.hpp"
#include "AssetManager.h"
#include <Renderer/GLRenderer.h>
#include "TransformComponent.h"
#include "EkActor.h"
#include "ComponentIds.h"

using namespace Eklavya::Renderer;
using namespace Eklavya::Asset;
namespace Eklavya
{
	RenderComponent::RenderComponent(EkActor& owner, const EMeshType meshType)
	    : EkComponent(owner, CoreComponentIds::RENDER_COMPONENT_ID)
	    , mMeshType(meshType)
	    , mMesh(meshType == EMeshType::SPHERE ? Asset::EBuiltInMesh::SPHERE : Asset::EBuiltInMesh::BOX)
	{
		UpdateBounds(mMesh.mMin, mMesh.mMax);
	}
	RenderComponent::RenderComponent(EkActor& owner, ModelID modelId)
	    : EkComponent(owner, CoreComponentIds::RENDER_COMPONENT_ID)
	    , mMeshType(EMeshType::ASSET)
	    , mModelID(modelId)
	{
	}
	void RenderComponent::Tick(float deltaTime)
	{
	}
	void RenderComponent::SetMesh(const Asset::GLMesh& mesh)
	{
		mMesh = mesh;
		mBound.mMin = mesh.mMin;
		mBound.mMax = mesh.mMax;
	}
	void RenderComponent::UpdateBounds(glm::vec3 min, glm::vec3 max)
	{
		mBound.mMin = glm::min(min, mBound.mMin);
		mBound.mMax = glm::max(max, mBound.mMax);
	}
} // namespace Eklavya
