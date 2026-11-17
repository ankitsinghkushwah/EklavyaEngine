//
//  StackOfBoxesDemo.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_STACK_OF_BOXES_DEMO
#define INC_STACK_OF_BOXES_DEMO

#include <MainEntryScene.hpp>
#include <AssetManager/MaterialInfo.h>
#include <Scene/EkActor.h>
#include "EkAudio.hpp"
#include <string>
#include <vector>

namespace Eklavya
{
	class StackOfBoxesDemo final : public MainEntryScene
	{
	public:
		StackOfBoxesDemo(class Director &pDirector);

		~StackOfBoxesDemo() override;

	private:
		enum class EStackShape { Grid, Pyramid };

		void LoadMaterials();

		void CreateFloor();

		void SetFlyMode(bool fly);

		void BuildStack();

		void BuildGrid();

		void BuildPyramid();

		void SpawnBox(const glm::vec3 &position);

		void ShootSphere();

		void ClearSpawned();

		void ResetScene();

		void TiltFloor(float deltaTime);

		void OnMouseAction(int key, int action) override;

		void OnKeyAction(int key, int action) override;

		void Tick(float deltaTime) override;

		void ImGuiProc() override;

	private:
		Eklavya::Audio mAudio;
		std::string mPopSound;
		Physics::EkBody *mFloor = nullptr;
		Asset::MaterialInfo mFloorMaterial;
		Asset::MaterialInfo mCrateMaterial;
		std::vector<EkActorID> mSpawned;
		bool mFlyMode = false;

		EStackShape mShape = EStackShape::Grid;
		int mColumns = 6;
		int mRows = 5;
		int mDepth = 1;
		int mPyramidBase = 6;
		int mPyramidDepth = 1;
		float mBoxSize = 20.0f;
		float mBoxMass = 30.0f;
		float mGap = 0.0f;
		float mDropHeight = 5.0f;
	};
} // namespace Eklavya

#endif
