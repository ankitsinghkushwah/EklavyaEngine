//
// TEMPORARY TOOL: collision sandbox for the box-box contact rewrite.
// Two boxes, no dynamics. Move them with ImGui and see what BoxVsBox returns.
//
// To remove: delete this folder and the three lines marked "COLLISION SANDBOX"
// in EklavyaEngine/MainEntryScene.cpp.
//

#ifndef INCCOLLISIONSANDBOXDEMO_H
#define INCCOLLISIONSANDBOXDEMO_H

#include <MainEntryScene.hpp>
#include <EkPhysics/CollisionSystem.h>
#include "SatTests.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Eklavya
{
	class CollisionSandboxDemo final : public MainEntryScene
	{
	public:
		CollisionSandboxDemo(class Director &pDirector);

		~CollisionSandboxDemo() override;

	private:
		struct SandboxBox
		{
			glm::vec3 position{0.0f};
			glm::vec3 eulerDegrees{0.0f};
			glm::vec3 size{40.0f};

			Physics::OBB ToOBB() const;
		};

		struct Result
		{
			bool colliding = false;
			float penetration = 0.0f;
			glm::vec3 normal{0.0f};
			std::vector<glm::vec3> points;
		};

		enum class EMode { BoxVsBox, ClipPolygon, Sat, SatTests, Faces, ClipSteps, EdgeEdge, Reduce };

		enum class ECheckState { Pass, Fail, Skipped };

		struct FaceCheck
		{
			std::string label;
			ECheckState state = ECheckState::Fail;
		};

		void RunFaces();

		void CheckFace(const Physics::BoxFace &face, const Physics::OBB &owner, const std::string &name);

		void AddFaceCheck(const std::string &label, bool pass);

		void DrawFacesMode(Renderer::DebugRenderer &debugRenderer);

		void ImGuiFacesResult();

		struct ClipCheck
		{
			int expectedCount = 0;
			float maxOutsideDistance = 0.0f;
			bool mainPass = false;
			bool emptyPass = false;
			bool allInPass = false;
			bool allOutPass = false;
		};

		// Clip steps mode (step 4): the 4 side-plane clips one at a time.
		void RunClipSteps();

		void DrawClipStepsMode(Renderer::DebugRenderer &debugRenderer);

		void ImGuiClipStepsResult();

		// Edge-edge mode (step 6): supporting edges, closest points, one contact.
		void RunEdgeEdge();

		void DrawEdgeEdgeMode(Renderer::DebugRenderer &debugRenderer);

		void ImGuiEdgeEdgeResult();

		// Reduce mode (step 7): full face manifold vs ReduceManifold's 4 points.
		void RunReduce();

		void DrawReduceMode(Renderer::DebugRenderer &debugRenderer);

		void ImGuiReduceResult();

		void RunClip();

		void DrawClipMode(Renderer::DebugRenderer &debugRenderer);

		void ImGuiClipMode();

		void ImGuiBoxMode();

		void ImGuiSatTestsMode();

		void SelectSatCase(int index);

		bool IsSatMode() const { return mMode == EMode::Sat || mMode == EMode::SatTests; }

		void ApplyPreset(int index);

		void RunCollision();

		void DrawBox(Renderer::DebugRenderer &debugRenderer, const SandboxBox &box, glm::vec4 color) const;

		void CopyPoseToClipboard() const;

		void SetFlyMode(bool fly);

		void OnKeyAction(int key, int action) override;

		void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;

		void ImGuiProc() override;

		SandboxBox mBoxA;
		SandboxBox mBoxB;
		Result mResult;

		// SAT mode (step 2)
		Physics::SatResult mSat;
		bool mSatSymmetric = false;

		// Faces mode (step 3)
		Physics::FacePair mFaces;
		bool mFacesValid = false; // overlapping and a face axis won
		std::vector<FaceCheck> mFaceChecks;

		// Clip steps mode (step 4)
		struct ClipStepPoint
		{
			glm::vec3 position{0.0f};
			bool isNew = false; // made on the plane by this clip, not one of the input vertices
		};

		bool mStepsValid = false; // overlapping and a face axis won
		Physics::ClipPlane mSidePlanes[4];
		std::vector<ClipStepPoint> mChained[5]; // [0] = incident face, [k] = after side plane k-1
		std::vector<ClipStepPoint> mFromOriginal[5]; // [k] = incident face clipped by side plane k-1 only
		std::vector<glm::vec3> mClipIncidentResult; // what ClipIncidentFace returned
		bool mClipIncidentMatches = false;
		int mClipStep = 1; // 0 = before any clip, 1..4 = after side plane 0..3
		bool mClipChained = true;

		// Edge-edge mode (step 6)
		bool mEdgeValid = false; // overlapping and an edge pair won
		Physics::BoxEdge mEdgeA; // supporting edge of the first box passed in
		Physics::BoxEdge mEdgeB; // supporting edge of the second box
		glm::vec3 mEdgePointA{0.0f}; // ClosestPointsBetweenEdges output on mEdgeA
		glm::vec3 mEdgePointB{0.0f}; // ClosestPointsBetweenEdges output on mEdgeB
		Physics::ContactManifold mEdgeManifold; // what BuildEdgeManifold returned
		float mEdgeBruteForceGap = 0.0f; // smallest |P - Q| from a grid search over both edges

		// Reduce mode (step 7)
		bool mReduceValid = false; // overlapping and a face axis won
		Physics::ContactManifold mReduceFull; // BuildFaceManifold output, 0 to 8 points
		Physics::ContactManifold mReduced; // ReduceManifold output
		std::vector<int> mReducedIndex; // for each reduced point, its index in mReduceFull (-1 if not found)
		float mReduceFullArea = 0.0f; // area of the polygon through all points
		float mReduceKeptArea = 0.0f; // area of the reduced polygon
		float mReduceBestArea = 0.0f; // largest area any 4 of the points can make (brute force)

		// SAT tests tab
		std::vector<SatTestCase> mSatCases;
		int mSelectedSatCase = 0;

		EMode mMode = EMode::ClipPolygon;

		// Clip polygon mode (step 1)
		std::vector<glm::vec3> mClipInput{
			glm::vec3(-40.0f, 90.0f, 0.0f), glm::vec3(30.0f, 95.0f, 0.0f),
			glm::vec3(45.0f, 50.0f, 0.0f), glm::vec3(-30.0f, 40.0f, 0.0f)
		};
		std::vector<glm::vec3> mClipOutput;
		Physics::ClipPlane mClipPlane;
		ClipCheck mClipCheck;
		float mClipAngleDegrees = 0.0f;
		float mClipOffset = 0.0f;

		bool mFlyMode = false;
		bool mSwapOrder = false;
		bool mDrawSolid = true;
		bool mDrawNormals = true;
		float mPointRadius = 1.2f;
		float mNormalLength = 15.0f;
	};
} // namespace Eklavya

#endif
