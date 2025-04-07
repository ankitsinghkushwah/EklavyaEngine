//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef BaseDemoScene_h
#define BaseDemoScene_h

#include <AssetManager/MaterialInfo.h>
#include <EkPhysics/PhysicsWorld.h>
#include <Renderer/Material.h>
#include <Scene/EkScene.h>

namespace Eklavya
{
	class Director;

	class MainEntryScene : public EkScene
	{
	public:
		MainEntryScene(Director& pDirector);

		~MainEntryScene() override;

		void CreateStage();

		UniqueActor CreateCube(glm::vec3 pos,
			glm::vec3 scale,
			glm::vec3 rotate,
			float mass,
			Asset::MaterialInfo matInfo,
			uint32_t groupFlag = Physics::OTHER_COLLIDABLES,
			bool kid = false);

		UniqueActor CreateSphere(glm::vec3 pos, float radius, float mass, Asset::MaterialInfo matInfo,
			uint32_t groupFlag = Physics::OTHER_COLLIDABLES, bool kid = false);

		void ImGuiProc() override;

#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer& debugRenderer) override;


#endif
	};
} // namespace Eklavya

#endif /* BaseDemoScene_h */
