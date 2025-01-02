//
//  RenderPass.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 16/06/2023.
//

#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <stdio.h>

#include "Material.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include "EkActor.h"
#include "RenderComponent.hpp"
#include "Framebuffer.hpp"

extern glm::vec3 lightPosition;

namespace Eklavya
{
	class RenderComponent;
	class EkScene;
} // namespace Eklavya

namespace Eklavya::Renderer
{
	class GLRenderer;
	class DebugRenderer;

	using ACTORS_MAP = std::array<std::vector<const RenderComponent*>, ERenderGroup::RG_MAX>;

	enum ERenderPass
	{
		RP_MIN = 0,
		SHADOW_PASS = RP_MIN,
		MAIN_PASS,
		RP_MAX
	};

	class IRenderPass
	{
	  public:
		IRenderPass(ERenderPass type);
		virtual ~IRenderPass();
		virtual bool Init() = 0;
		virtual void TryAddingActor(const RenderComponent& renderComponent) = 0;
		virtual void PreRun() = 0;
		virtual void Run(GLRenderer& renderer, const EkScene& scene) = 0;
		virtual void PostRun() = 0;

		ERenderPass GetType()
		{
			return mType;
		}

		std::unique_ptr<IFramebuffer> mFramebuffer;

#ifdef EKDEBUG
		virtual void DebugDraw(DebugRenderer&)
		{
		}
#endif
	  protected:
		ERenderPass mType;
	};

	class ShadowMapPass : public IRenderPass
	{
	  public:
		ShadowMapPass(int depthPrecision, int width, int height);
		~ShadowMapPass();

		bool Init() override;
		void TryAddingActor(const RenderComponent& renderComponent) override;
		void PreRun() override;
		void Run(GLRenderer& renderer, const EkScene& scene) override;
		void PostRun() override;

#ifdef EKDEBUG
		void DebugDraw(DebugRenderer&) override;
#endif

		glm::mat4 mLightPVMatrix;

	  private:
		int mDepthPrecision;
		int mWidth;
		int mHeight;

		SHARED_SHADER mShader;

		std::vector<const RenderComponent*> mActors;
	};

	class MainPass : public IRenderPass
	{
	  public:
		MainPass(int depth, int colorChannels, int width, int height);
		~MainPass();

		bool Init() override;
		void TryAddingActor(const RenderComponent& renderComponent) override;
		void PreRun() override;
		void Run(GLRenderer& renderer, const EkScene& scene) override;
		void PostRun() override;

#ifdef EKDEBUG
		void DebugDraw(DebugRenderer&) override;
#endif
	  private:
		int mDepthPrecision;
		int mWidth;
		int mHeight;

		ACTORS_MAP mActorsMap;
	};

} // namespace Eklavya::Renderer

#endif /* RenderPass_hpp */
