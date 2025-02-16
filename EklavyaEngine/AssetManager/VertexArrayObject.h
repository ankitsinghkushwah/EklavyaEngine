#ifndef _INC_HIP_HOP_VAO_H_
#define _INC_HIP_HOP_VAO_H_
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Eklavya
{
	class VertexArrayObject
	{
	  public:
		VertexArrayObject();
		~VertexArrayObject();
		template<typename Type = float>
		void Create(Type* bufferData, long bufferDataSize, int* Indices, size_t indicesSize = 0);
		void Destroy();
		void SetPosPtr(int size, int startOffset, int strideOffset);
		void SetNormalPtr(int startOffset, int strideOffset);
		void SetTexPtr(int startOffset, int strideOffset);
		// bone influence for animations
		void SetBoneIdsPtr(int startOffset, int strideOffset);
		void SetBoneWeightPtr(int startOffset, int strideOffset);

		void Bind() const { glBindVertexArray(m_VAO); }

		bool HasSharedVertices() const { return mIndicesSize != -1; }

		int GetIndicesSize() const { return mIndicesSize; }

		void Unbind() const { glBindVertexArray(0); }

		static void SetupVAOForSphere(VertexArrayObject& vao, glm::vec3& min, glm::vec3& max);
		static void SetupVAOForBox(VertexArrayObject& vao, glm::vec3& min, glm::vec3& max);

	  public:
		mutable GLuint m_VAO;
		GLuint         m_VBO;
		GLuint         m_EBO;
		int            mIndicesSize;
	};

	template<typename Type>
	void VertexArrayObject::Create(Type* bufferData, long bufferDataSize, int* indices, size_t indicesSize)
	{
		m_VBO = -1;
		m_EBO = -1;
		m_VAO = -1;
		glGenVertexArrays(1, &m_VAO);
		Bind();
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, bufferDataSize, bufferData, GL_STATIC_DRAW);
		if (indicesSize > 0)
		{
			mIndicesSize = indicesSize / sizeof(int);
			glGenBuffers(1, &m_EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
		}
		else
		{
			mIndicesSize = -1;
		}
		Unbind();
	}
} // namespace Eklavya
#endif
