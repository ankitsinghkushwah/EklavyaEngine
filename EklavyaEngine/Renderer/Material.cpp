#include <Renderer/Material.h>
#include <AssetManager/Texture2D.h>
#include <AssetManager/ShaderProgram.h>
#include <AssetManager/Cubemap.h>

namespace Eklavya::Renderer
{
	MATERIAL_ID IMaterial::s_IdCounter = 0;

	IMaterial::IMaterial(SHARED_SHADER shader, ERenderGroup state) : m_Shader(shader), m_State(state), mID(s_IdCounter++)
	{
	}

	IMaterial::~IMaterial()
	{
	}

	void IMaterial::Activate()
	{
	}

	void IMaterial::Deactivate()
	{
	}

	void IMaterial::Tick(float dt)
	{
	}

	// LitMaterial

	LitMaterial::LitMaterial(SHARED_SHADER shader) : IMaterial(shader, ERenderGroup::ACTOR)
	{
	}

	void LitMaterial::Activate()
	{
		m_Shader->Use();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);

		m_Active = true;
	}

	void LitMaterial::SetMaterialInfo(const Asset::MaterialInfo& info)
	{
		m_Shader->SetVec3("material.color", info.mBaseColor);
		m_Shader->SetFloat("material.opacity", info.mOpacity);
		m_Shader->SetFloat("material.specularStrength", info.mSpecularStrength);
		m_Shader->SetFloat("material.shininess", info.mShininess);
		m_Shader->SetInt("material.tiling", info.mTiling);

		m_Shader->SetInt("material.bAlbedo", info.mBaseMap != nullptr);
		m_Shader->SetInt("material.bNormal", info.mNormalMap != nullptr);

		// Units 0 and 1 hold the material maps; unit 8 is the shadow map (see MainPass).
		if (info.mBaseMap != nullptr)
		{
			m_Shader->SetInt("material.albedoMap", 0);
			info.mBaseMap->BindToUnit(0);
		}

		if (info.mNormalMap != nullptr)
		{
			m_Shader->SetInt("material.normalMap", 1);
			info.mNormalMap->BindToUnit(1);
		}
	}

	void LitMaterial::Deactivate()
	{
		m_Active = false;
	}

	// SkyBox

	SkyboxMaterial::SkyboxMaterial(SHARED_SHADER shader) : IMaterial(shader, ERenderGroup::SKYBOX)
	{
	}

	void SkyboxMaterial::Activate()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		m_Shader->Use();

		if (mCubemap != nullptr)
		{
			glActiveTexture(GL_TEXTURE0);
			m_Shader->SetInt("cubemap", 0);
			mCubemap->BindToUnit(0);
		}

		m_Active = true;
	}

	void SkyboxMaterial::Deactivate()
	{
		m_Active = false;
	}

} // namespace Eklavya::Renderer
