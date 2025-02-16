#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <array>
#include <AssetManager/Asset.h>
#include <AssetManager/MaterialInfo.h>
namespace Eklavya::Renderer
{
	// DON'T BREAK THE SEQUENCE
	enum ERenderGroup
	{
		SKYBOX = 0,
		ACTOR = 1,
		RG_MAX
	};
	using MATERIAL_ID = std::uint32_t;
	class IMaterial
	{
	  public:
		IMaterial(SHARED_SHADER shader, ERenderGroup state);
		virtual ~IMaterial();
		virtual void Activate();
		virtual void SetMaterialInfo(const Asset::MaterialInfo& info)
		{
		}
		virtual void Deactivate();
		virtual void Tick(float dt);
		bool         IsActive()
		{
			return m_Active;
		}
		ERenderGroup      m_State = ERenderGroup::ACTOR;
		SHARED_SHADER     m_Shader = nullptr;
		const MATERIAL_ID mID;

	  protected:
		bool               m_Active = false;
		static MATERIAL_ID s_IdCounter;
	};
	using SHARED_MATERIAL = std::shared_ptr<IMaterial>;
	struct PBRMaterial : public IMaterial
	{
		PBRMaterial(SHARED_SHADER shader);
		void Activate() override;
		void SetMaterialInfo(const Asset::MaterialInfo& info) override;
		void Deactivate() override;
		// variables
		SHARED_CUBEMAP m_Cubemap = nullptr;
	};
	using SHARED_PBR_MATERIAL = std::shared_ptr<PBRMaterial>;
	struct SkyboxMaterial : public IMaterial
	{
		SkyboxMaterial(SHARED_SHADER shader);
		void           Activate() override;
		void           Deactivate() override;
		SHARED_CUBEMAP mCubemap;
	};
	using SHARED_SKY_MATERIAL = std::shared_ptr<SkyboxMaterial>;
} // namespace Eklavya::Renderer
