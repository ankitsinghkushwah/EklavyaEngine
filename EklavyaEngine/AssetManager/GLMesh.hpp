//
//  GLMesh.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 27/05/2023.
//
#ifndef GLMesh_hpp
#define GLMesh_hpp
#include "VertexArrayObject.h"
#include <glm/glm.hpp>
#include <AssetManager/MaterialInfo.h>
#include <string>
#include <AssetManager/AnimationData.h>
#include <vector>

namespace Eklavya::Asset
{
	enum class EBuiltInMesh
	{
		SPHERE,
		BOX
	};
	constexpr uint32_t MAX_BONE_INFLUENCE = 4;
	// container to store bones which will influence vertices on mesh
	struct MeshVAOData
	{
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<float> weights;
		std::vector<float> boneIds;
		std::vector<int>   indices;
	};
	struct GLMesh
	{
	  public:
		GLMesh(EBuiltInMesh mesh_type);
		GLMesh();
		~GLMesh();
		void                     InitVAO(const MeshVAOData& meshData);
		const VertexArrayObject& VAO() const
		{
			return mVAO;
		}
		MaterialInfo      mMaterialInfo;
		uint32_t          mID;
		std::string       mName;
		VertexArrayObject mVAO;
		// bounds
		glm::vec3 mMin;
		glm::vec3 mMax;
		// influenced by bones
		std::vector<BoneInfo> mBones;
	};
} // namespace Eklavya::Asset
#endif /* GLMesh_hpp */
