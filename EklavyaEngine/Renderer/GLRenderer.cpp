#include "GLRenderer.h"
#include "../AssetManager/ShaderProgram.h"
#include <glad/glad.h>
#include "../Scene/EkScene.h"
#include "../GLFWGame.h"
#include "VertexArrayObject.h"
#include "AssetManager.h"
#include "CoreUtils/Logger.h"
#include "CoreUtils/Singleton.h"
#include "GLMesh.hpp"
#include "SpringFollowCamera.hpp"
#include "EkScene.h"
#include "imgui.h"
using namespace Eklavya::Asset;

namespace Eklavya::Renderer
{
	GLRenderer::GLRenderer(const GLWindowContext& context) : mContext(context), mOutputModel(1.0f)
	{
		for (int rp = 0; rp < mRenderPasses.size(); ++rp)
		{
			switch (rp)
			{
				case ERenderPass::SHADOW_PASS:
					mRenderPasses[rp] = std::make_unique<ShadowMapPass>(64, 1024.0f * 4, 1024.0f * 4);
					break;
				case ERenderPass::MAIN_PASS:
					mRenderPasses[rp] = std::make_unique<MainPass>(32, 4, mContext.GetWidth(), mContext.GetHeight());
					break;
			}
			mRenderPasses[rp]->Init();
		}
	}

	GLRenderer::~GLRenderer()
	{
	}

	void GLRenderer::LoadBuiltInMaterials()
	{
		SHARED_SHADER       skyShader = AssetManager::GetInstance().GetAsset<ShaderProgram>("skybox");
		SHARED_SKY_MATERIAL skyboxMat = std::make_shared<SkyboxMaterial>(skyShader);
		AddMaterialToState(Renderer::ERenderGroup::SKYBOX, skyboxMat);
		SHARED_SHADER       brdf = AssetManager::GetInstance().GetAsset<ShaderProgram>("standard_brdf");
		SHARED_PBR_MATERIAL pbrMat = std::make_shared<PBRMaterial>(brdf);
		AddMaterialToState(Renderer::ERenderGroup::ACTOR, pbrMat);
		mMainOutputShader = AssetManager::GetInstance().GetAsset<ShaderProgram>("main_output");
	}

	void GLRenderer::Init()
	{
		LoadBuiltInMaterials();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float quadVertices[] = {// vertex attributes for a quad that fills the entire screen in
		                        // Normalized Device Coordinates. positions   // texCoords
		                        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		                        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
		glBindVertexArray(0);
	}

	void GLRenderer::AddMaterialToState(ERenderGroup group, SHARED_MATERIAL material)
	{
		mGroupMaterials[group] = material;
	}

	void GLRenderer::AddActor(const RenderComponent& renderComponent)
	{
		for (auto& rp_ptr : mRenderPasses)
			rp_ptr->TryAddingActor(renderComponent);
	}

	void GLRenderer::PostPassesResult()
	{
		int width, height;
		glfwGetFramebufferSize(mContext.GetWindow(), &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mMainOutputShader->Use();
		GLint texture = mRenderPasses[ERenderPass::MAIN_PASS]->mFramebuffer->ColorOutput();
		mMainOutputShader->SetInt("mainOutputPass", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(mVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void GLRenderer::SetShaderDataForActor(const RenderComponent& renderComponent, SHARED_SHADER& shader)
	{
		if (renderComponent.mHasBones)
		{
			shader->SetInt("bApplyAnimation", 1);
			const std::vector<glm::mat4>& transforms = renderComponent.boneTransforms;

			for (int i = 0; i < MAX_BONES_SUPPORTED; ++i)
			{
				std::string n = "gBones[" + std::to_string(i) + "]";
				shader->SetMat4(n.c_str(), transforms[i]);
			}
		}
		else
		{
			shader->SetInt("bApplyAnimation", 0);
		}
	}

	void GLRenderer::RenderInternal(const std::vector<const RenderComponent*>& actors, SHARED_SHADER& shader, SHARED_MATERIAL material)
	{
		for (const RenderComponent* renderComponent : actors)
		{
			const VertexArrayObject& vao = renderComponent->GetMesh().VAO();
			vao.Bind();
			const glm::mat4& model = renderComponent->mWorldMatrix;
			if (material != nullptr)
			{
				material->SetMaterialInfo(renderComponent->GetMesh().mMaterialInfo);
			}
			shader->SetMat4("model", model);

			SetShaderDataForActor(*renderComponent, shader);

			if (vao.HasSharedVertices())
				glDrawElements(GL_TRIANGLES, vao.GetIndicesSize(), GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(GL_TRIANGLES, 0, 36);
			vao.Unbind();
		}
	}

	void GLRenderer::Render(EkScene& scene)
	{
		for (auto& pass : mRenderPasses)
		{
			pass->PreRun();
			pass->Run(*this, scene);
#ifdef EKDEBUG
			if (pass->GetType() == MAIN_PASS)
			{
				DebugDraw(scene);
			}
#endif
			pass->PostRun();
		}
		PostPassesResult();
	}

#ifdef EKDEBUG
	void GLRenderer::DebugDraw(EkScene& scene)
	{
		glm::mat4 projection = scene.CurrentCamera()->GetProjection();
		glm::mat4 view = scene.CurrentCamera()->GetView();
		mDebugRenderer.SetData(projection, view);
		scene.DebugDraw(mDebugRenderer);
		mDebugRenderer.DrawAddedShapes();
	}
#endif

	void GLRenderer::ImGuiProc()
	{
	}

} // namespace Eklavya::Renderer
