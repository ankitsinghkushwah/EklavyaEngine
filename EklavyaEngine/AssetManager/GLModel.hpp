#ifndef INC_GLMODEL
#define INC_GLMODEL
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Helpers.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "../CoreUtils/Logger.h"
#include "IAsset.h"
#include <AssetManager/ShaderProgram.h>
#include "GLMesh.hpp"
#include <memory>

namespace Eklavya::Asset
{
	struct ModelNode
	{
		std::string mName;
		std::vector<GLMesh *> mMeshes;
		glm::mat4 mTransform;
		glm::mat4 mWorldTransform;
		std::vector<std::shared_ptr<ModelNode> > mChildren;
		std::string mPropertyName = "";
		int mPropertyValue = -1;

		~ModelNode()
		{
			for (auto *mesh: mMeshes)
				delete mesh;
			mChildren.clear();
		}
	};

	struct GLModel : public IAsset
	{
		std::vector<GLMesh *> mSceneMeshes;
		std::shared_ptr<ModelNode> mRootNode = nullptr;
		std::vector<MaterialInfo> mMaterials;
		std::string mDirectory;
		unsigned m_ModelID;

	public:
		GLModel(const std::string &assetName, int modelID);

		~GLModel();

		void Load(const std::string &fullPath);

	private:
		std::shared_ptr<ModelNode> ProcessNode(aiNode &parentNode, const aiScene &scene, glm::mat4 parentTransform);

		GLMesh *ExtractMeshData(const aiMesh &mesh, const aiScene &scene);

		void LoadMaterials(const aiScene *scene);

		void SetVertexBoneData(MeshVAOData &meshData, int vertexIdx, int boneID, float weight);

		void ExtractBoneWeightForVertices(MeshVAOData &meshData, std::vector<BoneInfo> &bones, const aiMesh &mesh,
		                                  const aiScene &scene);
	};
} // namespace Eklavya::Asset
#endif
