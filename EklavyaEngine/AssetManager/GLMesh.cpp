//
//  GLMesh.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 27/05/2023.
//
#include "GLMesh.hpp"
#include <vector>

namespace Eklavya::Asset
{

	GLMesh::GLMesh(EBuiltInMesh meshType)
	{
		if (meshType == EBuiltInMesh::BOX)
			VertexArrayObject::SetupVAOForBox(mVAO, mMin, mMax);
		else if (meshType == EBuiltInMesh::SPHERE)
			VertexArrayObject::SetupVAOForSphere(mVAO, mMin, mMax);
	}

	GLMesh::GLMesh()
	{
	}

	GLMesh::~GLMesh()
	{
		mVAO.Destroy();
	}

	void GLMesh::InitVAO(const MeshVAOData& meshData)
	{
		std::vector<float> buffer;
		long               bufferSize = sizeof(float) * (meshData.vertices.size() + meshData.normals.size() + meshData.uvs.size() + meshData.weights.size());
		bufferSize += sizeof(int) * meshData.boneIds.size();
		// buffer.reserve(bufferSize);
		buffer.insert(buffer.end(), meshData.vertices.begin(), meshData.vertices.end());
		buffer.insert(buffer.end(), meshData.normals.begin(), meshData.normals.end());
		buffer.insert(buffer.end(), meshData.uvs.begin(), meshData.uvs.end());
		buffer.insert(buffer.end(), meshData.weights.begin(), meshData.weights.end());
		buffer.insert(buffer.end(), meshData.boneIds.begin(), meshData.boneIds.end());
		mVAO.Create(buffer.data(),
		            bufferSize,
		            (int*)&meshData.indices.data()[0],

		            sizeof(int) * meshData.indices.size());
		const int verticesWidth = static_cast<int>(sizeof(float) * meshData.vertices.size());
		const int normalsWidth = static_cast<int>(sizeof(float) * meshData.normals.size());
		const int uvsWidth = static_cast<int>(sizeof(float) * meshData.uvs.size());
		const int boneWeightWidth = static_cast<int>(sizeof(float) * meshData.weights.size());

		mVAO.SetPosPtr(3, 0, 0);
		mVAO.SetNormalPtr(verticesWidth, 0);
		mVAO.SetTexPtr(verticesWidth + normalsWidth, 0);
		mVAO.SetBoneWeightPtr(verticesWidth + normalsWidth + uvsWidth, 0);
		mVAO.SetBoneIdsPtr(verticesWidth + normalsWidth + uvsWidth + boneWeightWidth, 0);
	}
} // namespace Eklavya::Asset
