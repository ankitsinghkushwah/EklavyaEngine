//
// TEMPORARY TOOL: collision sandbox for the box-box contact rewrite.
// See the header for how to remove it.
//

#include "CollisionSandboxDemo.h"
#include "Director.hpp"
#include "imgui/imgui.h"
#include "glfw/glfw3.h"
#include <Renderer/DebugRenderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <cstdio>
#include <cfloat>
#include <cmath>
#include <string>

using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	namespace
	{
		const char *kPresets[] = {
			"1. Flat on top (expect 4)",
			"2. Yawed 45 on top (expect 4-8)",
			"3. Edge on face (expect 2)",
			"4. Corner on face (expect 1)",
			"5. Edge crossing edge (expect 1)",
			"6. Separated (expect none)",
			"7. Yawed 30 and offset: overhangs all 4 edges",
			"8. Yawed 45 and tilted 3: 8 points, uneven depths"
		};

		const glm::vec4 kColorA(0.20f, 0.45f, 0.90f, 0.30f);
		const glm::vec4 kColorB(0.95f, 0.60f, 0.15f, 0.30f);
		const glm::vec4 kColorPoint(1.0f, 0.1f, 0.1f, 1.0f);
		const glm::vec4 kColorNormal(0.1f, 1.0f, 0.2f, 1.0f);
		const glm::vec4 kColorReference(1.0f, 0.9f, 0.1f, 1.0f);
		const glm::vec4 kColorIncident(1.0f, 0.3f, 0.8f, 1.0f);
	} // namespace

	OBB CollisionSandboxDemo::SandboxBox::ToOBB() const
	{
		OBB obb;
		obb.center = position;
		obb.halfExtents = size * 0.5f;
		obb.rotation = glm::toMat3(glm::quat(glm::radians(eulerDegrees)));
		obb.worldTransform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(obb.rotation);
		return obb;
	}

	CollisionSandboxDemo::CollisionSandboxDemo(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		ApplyPreset(0);
		mSatCases = BuildSatTestCases();
		SetFlyMode(false);
	}

	CollisionSandboxDemo::~CollisionSandboxDemo() {}

	void CollisionSandboxDemo::SetFlyMode(bool fly)
	{
		mFlyMode = fly;
		mDirector.HideMouse(fly);
		CurrentCamera()->SetEnabled(fly);
	}

	void CollisionSandboxDemo::ApplyPreset(int index)
	{
		// Box A is 40 units wide and centred at y = 40, so its top face is at y = 60.
		// Every colliding preset sinks B about 2 units into A.
		mBoxA = SandboxBox{glm::vec3(0.0f, 40.0f, 0.0f), glm::vec3(0.0f), glm::vec3(40.0f)};
		mBoxB = SandboxBox{glm::vec3(0.0f, 78.0f, 0.0f), glm::vec3(0.0f), glm::vec3(40.0f)};

		switch (index)
		{
			case 0:
				break;
			case 1:
				mBoxB.eulerDegrees = glm::vec3(0.0f, 45.0f, 0.0f);
				break;
			case 2:
				mBoxB.eulerDegrees = glm::vec3(0.0f, 0.0f, 45.0f);
				mBoxB.position.y = 86.3f;
				break;
			case 3:
				mBoxB.eulerDegrees = glm::vec3(-45.0f, 0.0f, 35.264f);
				mBoxB.position.y = 92.6f;
				break;
			case 4:
				// Both boxes stand on an edge, and the two edges cross at right angles.
				mBoxA.eulerDegrees = glm::vec3(0.0f, 0.0f, 45.0f);
				mBoxB.eulerDegrees = glm::vec3(45.0f, 0.0f, 0.0f);
				mBoxB.position.y = 94.5f;
				break;
			case 5:
				mBoxB.position.y = 90.0f;
				break;
			case 6:
				// Same size, turned and nudged so B's bottom face sticks out past every edge of A's top face.
				mBoxB.eulerDegrees = glm::vec3(0.0f, 30.0f, 0.0f);
				mBoxB.position = glm::vec3(4.0f, 78.0f, 6.0f);
				break;
			case 7:
				// Octagon contact like preset 2, but tilted so every point has a different depth.
				mBoxB.eulerDegrees = glm::vec3(3.0f, 45.0f, 0.0f);
				mBoxB.position.y = 77.5f;
				break;
			default:
				break;
		}
	}

	void CollisionSandboxDemo::RunCollision()
	{
		mResult = Result{};

		const OBB a = mBoxA.ToOBB();
		const OBB b = mBoxB.ToOBB();

		if (mMode == EMode::Faces)
		{
			RunFaces();
			return;
		}

		if (mMode == EMode::ClipSteps)
		{
			RunClipSteps();
			return;
		}

		if (mMode == EMode::EdgeEdge)
		{
			RunEdgeEdge();
			return;
		}

		if (mMode == EMode::Reduce)
		{
			RunReduce();
			return;
		}

		if (IsSatMode())
		{
			mSat = mSwapOrder ? BoxBoxSAT(b, a) : BoxBoxSAT(a, b);

			// Swapping the arguments must give the same answer with the normal reversed.
			const SatResult other = mSwapOrder ? BoxBoxSAT(a, b) : BoxBoxSAT(b, a);
			mSatSymmetric = mSat.overlapping == other.overlapping;
			if (mSat.overlapping && other.overlapping)
			{
				mSatSymmetric = glm::abs(mSat.penetration - other.penetration) < 0.001f &&
				                glm::length(mSat.normal + other.normal) < 0.001f;
			}
			return;
		}

		ContactManifold manifold;
		mResult.colliding = mSwapOrder
			                    ? CollisionSystem::BoxVsBox(b, a, manifold)
			                    : CollisionSystem::BoxVsBox(a, b, manifold);
		mResult.normal = manifold.normal;
		for (const ContactPoint &point: manifold.points)
		{
			mResult.points.push_back(point.position);
			mResult.penetration = glm::max(mResult.penetration, point.depth);
		}
	}

	void CollisionSandboxDemo::AddFaceCheck(const std::string &label, bool pass)
	{
		mFaceChecks.push_back(FaceCheck{label, pass ? ECheckState::Pass : ECheckState::Fail});
	}

	// Checks one face against the box it should belong to, without rebuilding the face:
	// each property is tested on the vertices that came back.
	void CollisionSandboxDemo::CheckFace(const BoxFace &face, const OBB &owner, const std::string &name)
	{
		const float eps = 0.01f;

		const bool axisOk = face.axis >= 0 && face.axis <= 2 && (face.sign == 1.0f || face.sign == -1.0f);
		AddFaceCheck(name + ": axis is 0..2 and sign is +1 or -1", axisOk);
		if (axisOk == false)
		{
			return;
		}

		const glm::vec3 h = owner.halfExtents;
		const glm::vec3 outward = owner.rotation[face.axis] * face.sign;

		AddFaceCheck(name + ": normal is the box axis times sign", glm::length(face.normal - outward) < 0.001f);

		// Vertices in the box's local space, where a corner is (+-hx, +-hy, +-hz).
		glm::vec3 local[4];
		bool corners = true;
		for (int i = 0; i < 4; i++)
		{
			local[i] = glm::transpose(owner.rotation) * (face.vertices[i] - owner.center);
			for (int k = 0; k < 3; k++)
			{
				const float error = k == face.axis
					                    ? glm::abs(local[i][k] - face.sign * h[k])
					                    : glm::abs(glm::abs(local[i][k]) - h[k]);
				if (error > eps) corners = false;
			}
		}
		AddFaceCheck(name + ": the 4 vertices are corners of that face", corners);

		// Going round the perimeter, each step changes exactly one local coordinate.
		bool perimeter = true;
		for (int i = 0; i < 4; i++)
		{
			const glm::vec3 diff = local[i] - local[(i + 1) % 4];
			int changed = 0;
			for (int k = 0; k < 3; k++)
			{
				if (glm::abs(diff[k]) > eps) changed++;
			}
			if (changed != 1) perimeter = false;
		}
		AddFaceCheck(name + ": consecutive vertices share an edge", perimeter);

		const glm::vec3 windingNormal = glm::cross(face.vertices[1] - face.vertices[0],
		                                           face.vertices[2] - face.vertices[0]);
		AddFaceCheck(name + ": counter-clockwise seen from outside", glm::dot(windingNormal, outward) > 0.0f);

		const glm::vec3 expectedCenter = owner.center + outward * h[face.axis];
		AddFaceCheck(name + ": center is the middle of the face", glm::length(face.center - expectedCenter) < eps);
	}

	void CollisionSandboxDemo::RunFaces()
	{
		mFaceChecks.clear();
		mFacesValid = false;

		const OBB a = mBoxA.ToOBB();
		const OBB b = mBoxB.ToOBB();
		const OBB &first = mSwapOrder ? b : a;
		const OBB &second = mSwapOrder ? a : b;

		mSat = BoxBoxSAT(first, second);
		if (mSat.overlapping == false || mSat.type == ESatAxis::EdgeEdge)
		{
			return;
		}

		mFacesValid = true;
		mFaces = SelectFaces(first, second, mSat);

		// Which box should own the reference face comes from the SAT result, not from mFaces.flipped,
		// so a wrong flag cannot hide a wrong face.
		const bool expectFlipped = mSat.type == ESatAxis::FaceB;
		const OBB &referenceOwner = expectFlipped ? second : first;
		const OBB &incidentOwner = expectFlipped ? first : second;
		const int winningIndex = expectFlipped ? mSat.indexB : mSat.indexA;
		const glm::vec3 towardsOther = expectFlipped ? -mSat.normal : mSat.normal;

		AddFaceCheck("flipped is true exactly when the axis is FaceB", mFaces.flipped == expectFlipped);

		CheckFace(mFaces.reference, referenceOwner, "Reference");
		AddFaceCheck("Reference: on the winning axis", mFaces.reference.axis == winningIndex);
		AddFaceCheck("Reference: normal points at the other box",
		             glm::dot(mFaces.reference.normal, towardsOther) > 0.999f);

		CheckFace(mFaces.incident, incidentOwner, "Incident");

		// Try all 6 faces of the other box: none may be more opposed than the one chosen.
		float mostOpposed = FLT_MAX;
		for (int i = 0; i < 3; i++)
		{
			const float d = glm::dot(incidentOwner.rotation[i], towardsOther);
			mostOpposed = glm::min(mostOpposed, glm::min(d, -d));
		}
		AddFaceCheck("Incident: no face of that box is more opposed to the reference",
		             glm::dot(mFaces.incident.normal, towardsOther) < mostOpposed + 0.001f);

		// Swapping the arguments should pick the same two physical faces. This can only be compared
		// when both calls choose the same physical axis; when a face of A ties with a face of B,
		// each call picks its own first box and the check is skipped.
		FaceCheck swapCheck{"Swapped call picks the same two faces, flipped inverted", ECheckState::Skipped};
		const SatResult other = BoxBoxSAT(second, first);
		if (other.overlapping && other.type != ESatAxis::EdgeEdge && other.type != mSat.type)
		{
			const int otherIndex = other.type == ESatAxis::FaceB ? other.indexB : other.indexA;
			if (otherIndex == winningIndex)
			{
				const FacePair swapped = SelectFaces(second, first, other);
				const bool same = swapped.flipped != mFaces.flipped &&
				                  swapped.reference.axis == mFaces.reference.axis &&
				                  swapped.reference.sign == mFaces.reference.sign &&
				                  swapped.incident.axis == mFaces.incident.axis &&
				                  swapped.incident.sign == mFaces.incident.sign;
				swapCheck.state = same ? ECheckState::Pass : ECheckState::Fail;
			}
		}
		mFaceChecks.push_back(swapCheck);
	}

	void CollisionSandboxDemo::RunClipSteps()
	{
		mStepsValid = false;
		for (int k = 0; k < 5; k++)
		{
			mChained[k].clear();
			mFromOriginal[k].clear();
		}
		mClipIncidentResult.clear();

		const OBB a = mBoxA.ToOBB();
		const OBB b = mBoxB.ToOBB();
		const OBB &first = mSwapOrder ? b : a;
		const OBB &second = mSwapOrder ? a : b;

		mSat = BoxBoxSAT(first, second);
		if (mSat.overlapping == false || mSat.type == ESatAxis::EdgeEdge)
		{
			return;
		}

		mStepsValid = true;
		mFaces = SelectFaces(first, second, mSat);
		const BoxFace &ref = mFaces.reference;

		// The tool builds its own side planes, and forces them outward with a centre check,
		// so it does not depend on the winding of GetBoxFace.
		for (int i = 0; i < 4; i++)
		{
			const glm::vec3 v0 = ref.vertices[i];
			const glm::vec3 v1 = ref.vertices[(i + 1) % 4];
			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, ref.normal));
			const glm::vec3 middle = (v0 + v1) * 0.5f;
			if (glm::dot(normal, middle - ref.center) < 0.0f) normal = -normal;
			mSidePlanes[i] = ClipPlane{middle, normal};
		}

		// Tags each output point as new when it is not one of the input vertices.
		auto clipTagged = [](const std::vector<ClipStepPoint> &input, const ClipPlane &plane)
		{
			std::vector<glm::vec3> raw;
			for (const ClipStepPoint &p: input) raw.push_back(p.position);

			std::vector<ClipStepPoint> out;
			for (const glm::vec3 &q: ClipPolygonAgainstPlane(raw, plane))
			{
				bool isNew = true;
				for (const glm::vec3 &r: raw)
				{
					if (glm::length(q - r) < 0.001f) isNew = false;
				}
				out.push_back(ClipStepPoint{q, isNew});
			}
			return out;
		};

		for (const glm::vec3 &v: mFaces.incident.vertices)
		{
			mChained[0].push_back(ClipStepPoint{v, false});
		}
		mFromOriginal[0] = mChained[0];

		for (int k = 1; k <= 4; k++)
		{
			mChained[k] = clipTagged(mChained[k - 1], mSidePlanes[k - 1]);
			mFromOriginal[k] = clipTagged(mChained[0], mSidePlanes[k - 1]);
		}

		// Your ClipIncidentFace should give the same polygon as the last chained step.
		mClipIncidentResult = ClipIncidentFace(mFaces);
		mClipIncidentMatches = mClipIncidentResult.size() == mChained[4].size();
		for (const ClipStepPoint &p: mChained[4])
		{
			bool found = false;
			for (const glm::vec3 &q: mClipIncidentResult)
			{
				if (glm::length(p.position - q) < 0.01f) found = true;
			}
			if (found == false) mClipIncidentMatches = false;
		}
	}

	void CollisionSandboxDemo::DrawClipStepsMode(DebugRenderer &debugRenderer)
	{
		if (mStepsValid == false)
		{
			return;
		}

		const glm::vec4 inputColor(1.0f, 0.55f, 0.15f, 1.0f);
		const glm::vec4 outputColor(0.1f, 0.9f, 0.5f, 1.0f);
		const glm::vec4 newPointColor(0.65f, 0.45f, 1.0f, 1.0f);
		const glm::vec4 currentPlaneColor(0.65f, 0.45f, 1.0f, 1.0f);
		const glm::vec4 donePlaneColor(0.5f, 0.5f, 0.5f, 1.0f);
		const glm::vec4 outsideColor(1.0f, 0.15f, 0.15f, 1.0f);

		const BoxFace &ref = mFaces.reference;
		for (int i = 0; i < 4; i++)
		{
			debugRenderer.DrawLine(ref.vertices[i], ref.vertices[(i + 1) % 4], kColorReference, 1.0f, true);
		}

		// A side plane drawn as a wall standing on its edge, with an arrow along its normal.
		auto drawWall = [&](int i, const glm::vec4 &color, float thickness, bool arrow)
		{
			const glm::vec3 v0 = ref.vertices[i];
			const glm::vec3 v1 = ref.vertices[(i + 1) % 4];
			const glm::vec3 along = (v1 - v0) * 0.2f;
			const glm::vec3 up = ref.normal * 14.0f;
			const glm::vec3 c[4] = {v0 - along - up, v1 + along - up, v1 + along + up, v0 - along + up};
			for (int j = 0; j < 4; j++)
			{
				debugRenderer.DrawLine(c[j], c[(j + 1) % 4], color, thickness, true);
			}
			debugRenderer.DrawLine(c[0], c[2], color, thickness * 0.5f, true);
			debugRenderer.DrawLine(c[1], c[3], color, thickness * 0.5f, true);
			if (arrow)
			{
				const ClipPlane &plane = mSidePlanes[i];
				debugRenderer.DrawLine(plane.point, plane.point + plane.normal * 10.0f, color, 0.8f, true);
				debugRenderer.DrawSphere(plane.point + plane.normal * 10.0f, mPointRadius * 0.7f, color);
			}
		};

		auto drawPolygon = [&](const std::vector<ClipStepPoint> &poly, const glm::vec4 &color, float thickness)
		{
			for (size_t i = 0; i < poly.size(); i++)
			{
				debugRenderer.DrawLine(poly[i].position, poly[(i + 1) % poly.size()].position, color, thickness, true);
			}
		};

		auto outsideAnyPlane = [&](const glm::vec3 &p)
		{
			for (const ClipPlane &plane: mSidePlanes)
			{
				if (glm::dot(p - plane.point, plane.normal) > 0.01f) return true;
			}
			return false;
		};

		const int k = mClipStep;
		if (k == 0)
		{
			drawPolygon(mChained[0], inputColor, 1.0f);
			for (const ClipStepPoint &p: mChained[0]) debugRenderer.DrawSphere(p.position, mPointRadius, inputColor);
			return;
		}

		for (int i = 0; i < 4; i++)
		{
			if (i == k - 1) drawWall(i, currentPlaneColor, 0.8f, true);
			else if (mClipChained && i < k - 1) drawWall(i, donePlaneColor, 0.3f, false);
		}

		const std::vector<ClipStepPoint> &input = mClipChained ? mChained[k - 1] : mChained[0];
		const std::vector<ClipStepPoint> &output = mClipChained ? mChained[k] : mFromOriginal[k];

		drawPolygon(input, inputColor, 0.4f);
		drawPolygon(output, outputColor, 1.2f);

		if (mClipChained == false && k == 4)
		{
			// All four separate results, to show what appending them would give.
			for (int j = 1; j < 4; j++) drawPolygon(mFromOriginal[j], outputColor, 0.4f);
		}

		for (const ClipStepPoint &p: output)
		{
			glm::vec4 color = p.isNew ? newPointColor : outputColor;
			if (mClipChained == false && outsideAnyPlane(p.position)) color = outsideColor;
			debugRenderer.DrawSphere(p.position, mPointRadius * (p.isNew ? 1.3f : 1.0f), color);
		}
	}

	void CollisionSandboxDemo::ImGuiClipStepsResult()
	{
		const ImVec4 green(0.3f, 1.0f, 0.3f, 1.0f);
		const ImVec4 red(1.0f, 0.3f, 0.3f, 1.0f);
		const ImVec4 orange(1.0f, 0.55f, 0.15f, 1.0f);
		const ImVec4 purple(0.65f, 0.45f, 1.0f, 1.0f);

		if (mStepsValid == false)
		{
			ImGui::Text(mSat.overlapping ? "An edge pair won: no reference face (step 6)." : "Not overlapping.");
			return;
		}

		ImGui::Checkbox("Chained: each clip takes the previous output (correct)", &mClipChained);
		ImGui::SliderInt("Step", &mClipStep, 0, 4, mClipStep == 0 ? "start" : "after %d clips");
		if (mClipStep > 0)
		{
			ImGui::SameLine();
			ImGui::Text("(plane %d)", mClipStep - 1);
		}

		ImGui::TextColored(ImVec4(kColorReference.r, kColorReference.g, kColorReference.b, 1.0f), "Yellow: reference face");
		ImGui::TextColored(orange, "Orange: polygon going into this clip");
		ImGui::TextColored(purple, "Purple wall + arrow: current side plane (arrow = outside)");
		ImGui::TextColored(green, "Green: polygon coming out. Big purple spheres: new points");
		if (mClipChained)
		{
			ImGui::Text("Grey walls: planes already applied");
		}
		else
		{
			ImGui::TextColored(red, "Red spheres: outside some other side plane (off the face!)");
		}
		ImGui::Separator();

		if (mClipChained)
		{
			ImGui::Text("Points:  %d -> %d -> %d -> %d -> %d", static_cast<int>(mChained[0].size()),
			            static_cast<int>(mChained[1].size()), static_cast<int>(mChained[2].size()),
			            static_cast<int>(mChained[3].size()), static_cast<int>(mChained[4].size()));
		}
		else
		{
			int total = 0;
			int outside = 0;
			for (int j = 1; j <= 4; j++)
			{
				total += static_cast<int>(mFromOriginal[j].size());
				for (const ClipStepPoint &p: mFromOriginal[j])
				{
					for (const ClipPlane &plane: mSidePlanes)
					{
						if (glm::dot(p.position - plane.point, plane.normal) > 0.01f)
						{
							outside++;
							break;
						}
					}
				}
			}
			ImGui::Text("Each from the original: %d, %d, %d, %d points", static_cast<int>(mFromOriginal[1].size()),
			            static_cast<int>(mFromOriginal[2].size()), static_cast<int>(mFromOriginal[3].size()),
			            static_cast<int>(mFromOriginal[4].size()));
			ImGui::TextColored(red, "Appended together: %d points, %d of them off the reference face", total, outside);
		}

		const std::vector<ClipStepPoint> &shown = mClipChained ? mChained[mClipStep] : mFromOriginal[mClipStep];
		for (size_t i = 0; i < shown.size(); i++)
		{
			const glm::vec3 &p = shown[i].position;
			ImGui::Text("  %s%d (%.2f, %.2f, %.2f)", shown[i].isNew ? "new " : "    ", static_cast<int>(i), p.x, p.y,
			            p.z);
		}
		ImGui::Separator();

		ImGui::TextColored(mClipIncidentMatches ? green : red, "%s  ClipIncidentFace gives the chained result (%d points)",
		                   mClipIncidentMatches ? "PASS" : "FAIL", static_cast<int>(mClipIncidentResult.size()));
	}

	void CollisionSandboxDemo::RunEdgeEdge()
	{
		mEdgeValid = false;
		mEdgeManifold = ContactManifold{};

		const OBB a = mBoxA.ToOBB();
		const OBB b = mBoxB.ToOBB();
		const OBB &first = mSwapOrder ? b : a;
		const OBB &second = mSwapOrder ? a : b;

		mSat = BoxBoxSAT(first, second);
		if (mSat.overlapping == false || mSat.type != ESatAxis::EdgeEdge)
		{
			return;
		}

		mEdgeValid = true;

		// The same edges BuildEdgeManifold picks, fetched again so they can be drawn.
		mEdgeA = GetSupportingEdge(first, mSat.indexA, mSat.normal);
		mEdgeB = GetSupportingEdge(second, mSat.indexB, -mSat.normal);
		ClosestPointsBetweenEdges(mEdgeA, mEdgeB, mEdgePointA, mEdgePointB);
		mEdgeManifold = BuildEdgeManifold(first, second, mSat);

		// Reference answer: try every pair on a grid along both edges, keep the shortest gap.
		const int kSamples = 200;
		mEdgeBruteForceGap = FLT_MAX;
		for (int i = 0; i <= kSamples; i++)
		{
			const float s = -mEdgeA.halfLength + 2.0f * mEdgeA.halfLength * i / kSamples;
			const glm::vec3 p = mEdgeA.midpoint + s * mEdgeA.direction;
			for (int j = 0; j <= kSamples; j++)
			{
				const float t = -mEdgeB.halfLength + 2.0f * mEdgeB.halfLength * j / kSamples;
				const glm::vec3 q = mEdgeB.midpoint + t * mEdgeB.direction;
				mEdgeBruteForceGap = glm::min(mEdgeBruteForceGap, glm::length(p - q));
			}
		}
	}

	void CollisionSandboxDemo::DrawEdgeEdgeMode(DebugRenderer &debugRenderer)
	{
		if (mEdgeValid == false)
		{
			return;
		}

		const glm::vec4 edgeColorA(kColorA.r, kColorA.g, kColorA.b, 1.0f);
		const glm::vec4 edgeColorB(kColorB.r, kColorB.g, kColorB.b, 1.0f);
		const glm::vec4 gapColor(1.0f, 0.9f, 0.1f, 1.0f);

		auto drawEdge = [&](const BoxEdge &edge, const glm::vec4 &color)
		{
			const glm::vec3 from = edge.midpoint - edge.direction * edge.halfLength;
			const glm::vec3 to = edge.midpoint + edge.direction * edge.halfLength;
			debugRenderer.DrawLine(from, to, color, 0.8f, true);
			debugRenderer.DrawSphere(edge.midpoint, mPointRadius * 0.6f, color);
		};

		// The two supporting edges, with their midpoints as small spheres.
		drawEdge(mEdgeA, edgeColorA);
		drawEdge(mEdgeB, edgeColorB);

		// Closest points on each edge and the gap between them.
		debugRenderer.DrawSphere(mEdgePointA, mPointRadius, edgeColorA);
		debugRenderer.DrawSphere(mEdgePointB, mPointRadius, edgeColorB);
		debugRenderer.DrawLine(mEdgePointA, mEdgePointB, gapColor, 0.4f, true);

		// The contact BuildEdgeManifold produced, with the manifold normal.
		for (const ContactPoint &contact: mEdgeManifold.points)
		{
			debugRenderer.DrawSphere(contact.position, mPointRadius * 1.3f, kColorPoint);
			if (mDrawNormals)
			{
				debugRenderer.DrawLine(contact.position, contact.position + mEdgeManifold.normal * mNormalLength,
				                       kColorNormal, 0.4f, true);
			}
		}
	}

	void CollisionSandboxDemo::ImGuiEdgeEdgeResult()
	{
		const ImVec4 green(0.3f, 1.0f, 0.3f, 1.0f);
		const ImVec4 red(1.0f, 0.3f, 0.3f, 1.0f);

		if (mEdgeValid == false)
		{
			ImGui::Text(mSat.overlapping ? "A face axis won: no edge-edge contact (try preset 5)." : "Not overlapping.");
			return;
		}

		auto passFail = [&](bool pass, const char *label)
		{
			ImGui::TextColored(pass ? green : red, "%s  %s", pass ? "PASS" : "FAIL", label);
		};

		// Where a point sits along its edge, measured from the midpoint.
		auto along = [](const BoxEdge &edge, const glm::vec3 &p) { return glm::dot(p - edge.midpoint, edge.direction); };

		const float sA = along(mEdgeA, mEdgePointA);
		const float tB = along(mEdgeB, mEdgePointB);
		const float gap = glm::length(mEdgePointA - mEdgePointB);
		const glm::vec3 w = mEdgePointA - mEdgePointB;

		ImGui::TextColored(ImVec4(kColorA.r, kColorA.g, kColorA.b, 1.0f), "Edge A: axis %d, s = %.3f of +-%.2f",
		                   mSat.indexA, sA, mEdgeA.halfLength);
		ImGui::TextColored(ImVec4(kColorB.r, kColorB.g, kColorB.b, 1.0f), "Edge B: axis %d, t = %.3f of +-%.2f",
		                   mSat.indexB, tB, mEdgeB.halfLength);
		ImGui::Text("Gap |pA - pB|: %.4f   brute force: %.4f", gap, mEdgeBruteForceGap);
		ImGui::Text("Lean: w.dA = %.4f   w.dB = %.4f", glm::dot(w, mEdgeA.direction), glm::dot(w, mEdgeB.direction));
		ImGui::Text("SAT penetration: %.4f", mSat.penetration);
		ImGui::Text("Normal: (%.3f, %.3f, %.3f)", mEdgeManifold.normal.x, mEdgeManifold.normal.y,
		            mEdgeManifold.normal.z);
		for (size_t i = 0; i < mEdgeManifold.points.size(); i++)
		{
			const ContactPoint &c = mEdgeManifold.points[i];
			ImGui::Text("  p%d (%.2f, %.2f, %.2f)  depth %.4f", static_cast<int>(i), c.position.x, c.position.y,
			            c.position.z, c.depth);
		}
		ImGui::Separator();

		const float kTolerance = 0.01f;
		const bool onePoint = mEdgeManifold.points.size() == 1;
		passFail(onePoint, "exactly 1 contact point");
		passFail(glm::abs(sA) <= mEdgeA.halfLength + kTolerance && glm::abs(tB) <= mEdgeB.halfLength + kTolerance,
		         "both closest points are on their edges");
		// The grid step limits how close the brute force can get, so allow a little slack.
		const float gridSlack = (mEdgeA.halfLength + mEdgeB.halfLength) * 2.0f / 200.0f;
		passFail(gap <= mEdgeBruteForceGap + kTolerance && gap >= mEdgeBruteForceGap - gridSlack,
		         "gap matches the brute-force search");
		if (onePoint)
		{
			const ContactPoint &c = mEdgeManifold.points[0];
			passFail(glm::length(c.position - (mEdgePointA + mEdgePointB) * 0.5f) < kTolerance,
			         "contact is halfway between the closest points");
			passFail(c.depth > 0.0f, "depth > 0 (penetrating)");
			passFail(glm::abs(c.depth - mSat.penetration) < kTolerance, "depth equals SAT penetration");
			passFail(glm::length(mEdgeManifold.normal - mSat.normal) < 0.001f, "normal is SAT's normal (A to B)");
		}
	}

	namespace
	{
		// Area of a convex polygon (points in order), measured in the plane with normal n.
		float PolygonArea(const std::vector<glm::vec3> &poly, const glm::vec3 &n)
		{
			glm::vec3 sum(0.0f);
			for (size_t i = 0; i < poly.size(); i++)
			{
				sum += glm::cross(poly[i], poly[(i + 1) % poly.size()]);
			}
			return 0.5f * glm::abs(glm::dot(sum, n));
		}
	} // namespace

	void CollisionSandboxDemo::RunReduce()
	{
		mReduceValid = false;
		mReduceFull = ContactManifold{};
		mReduced = ContactManifold{};
		mReducedIndex.clear();
		mReduceFullArea = mReduceKeptArea = mReduceBestArea = 0.0f;

		const OBB a = mBoxA.ToOBB();
		const OBB b = mBoxB.ToOBB();
		const OBB &first = mSwapOrder ? b : a;
		const OBB &second = mSwapOrder ? a : b;

		mSat = BoxBoxSAT(first, second);
		if (mSat.overlapping == false || mSat.type == ESatAxis::EdgeEdge)
		{
			return;
		}

		mReduceValid = true;
		mFaces = SelectFaces(first, second, mSat);
		mReduceFull = BuildFaceManifold(mFaces, mSat, ClipIncidentFace(mFaces));
		mReduced = ReduceManifold(mReduceFull);

		for (const ContactPoint &kept: mReduced.points)
		{
			int found = -1;
			for (size_t i = 0; i < mReduceFull.points.size(); i++)
			{
				if (glm::length(kept.position - mReduceFull.points[i].position) < 0.0001f) found = static_cast<int>(i);
			}
			mReducedIndex.push_back(found);
		}

		// The full points are still in clip order, so they form a convex polygon.
		std::vector<glm::vec3> full;
		for (const ContactPoint &c: mReduceFull.points) full.push_back(c.position);
		std::vector<glm::vec3> kept;
		for (const ContactPoint &c: mReduced.points) kept.push_back(c.position);
		mReduceFullArea = PolygonArea(full, mSat.normal);
		mReduceKeptArea = PolygonArea(kept, mSat.normal);

		// Brute force: any 4 points taken in polygon order form a convex quad; keep the largest.
		const int n = static_cast<int>(full.size());
		if (n <= 4)
		{
			mReduceBestArea = mReduceFullArea;
			return;
		}
		for (int i = 0; i < n; i++)
			for (int j = i + 1; j < n; j++)
				for (int k = j + 1; k < n; k++)
					for (int l = k + 1; l < n; l++)
					{
						mReduceBestArea = glm::max(mReduceBestArea,
						                           PolygonArea({full[i], full[j], full[k], full[l]}, mSat.normal));
					}
	}

	void CollisionSandboxDemo::DrawReduceMode(DebugRenderer &debugRenderer)
	{
		if (mReduceValid == false)
		{
			return;
		}

		const glm::vec4 fullColor(0.7f, 0.7f, 0.7f, 1.0f);
		const glm::vec4 keptColor(1.0f, 0.9f, 0.1f, 1.0f);

		// All points from BuildFaceManifold: grey polygon, small grey spheres.
		const auto &full = mReduceFull.points;
		for (size_t i = 0; i < full.size(); i++)
		{
			debugRenderer.DrawLine(full[i].position, full[(i + 1) % full.size()].position, fullColor, 0.3f, true);
			debugRenderer.DrawSphere(full[i].position, mPointRadius * 0.7f, fullColor);
		}

		// What ReduceManifold kept: yellow polygon, red spheres, normals.
		const auto &kept = mReduced.points;
		for (size_t i = 0; i < kept.size(); i++)
		{
			debugRenderer.DrawLine(kept[i].position, kept[(i + 1) % kept.size()].position, keptColor, 0.8f, true);
			debugRenderer.DrawSphere(kept[i].position, mPointRadius * 1.4f, kColorPoint);
			if (mDrawNormals)
			{
				debugRenderer.DrawLine(kept[i].position, kept[i].position + mReduced.normal * mNormalLength,
				                       kColorNormal, 0.4f, true);
			}
		}
	}

	void CollisionSandboxDemo::ImGuiReduceResult()
	{
		const ImVec4 green(0.3f, 1.0f, 0.3f, 1.0f);
		const ImVec4 red(1.0f, 0.3f, 0.3f, 1.0f);

		if (mReduceValid == false)
		{
			ImGui::Text(mSat.overlapping ? "An edge pair won: no face manifold (try preset 8)." : "Not overlapping.");
			return;
		}

		auto passFail = [&](bool pass, const char *label)
		{
			ImGui::TextColored(pass ? green : red, "%s  %s", pass ? "PASS" : "FAIL", label);
		};

		const char *tags[] = {"#1 deepest", "#3 +area", "#2 farthest", "#4 -area"};
		const auto &full = mReduceFull.points;
		const auto &kept = mReduced.points;
		const bool reduced = full.size() > 4;

		ImGui::Text("Grey: all %d points from BuildFaceManifold.", static_cast<int>(full.size()));
		ImGui::Text("Red + yellow: the %d points ReduceManifold kept.", static_cast<int>(kept.size()));
		for (size_t i = 0; i < full.size(); i++)
		{
			int slot = -1;
			for (size_t k = 0; k < mReducedIndex.size(); k++)
			{
				if (mReducedIndex[k] == static_cast<int>(i)) slot = static_cast<int>(k);
			}
			const ContactPoint &c = full[i];
			ImGui::TextColored(slot >= 0 ? ImVec4(1.0f, 0.9f, 0.1f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
			                   "  p%d (%.2f, %.2f, %.2f)  depth %.3f  %s", static_cast<int>(i), c.position.x,
			                   c.position.y, c.position.z, c.depth, slot >= 0 && reduced ? tags[slot] : "");
		}
		ImGui::Text("Area: all points %.1f | kept %.1f (%.0f%%) | best 4 %.1f", mReduceFullArea, mReduceKeptArea,
		            mReduceFullArea > 0.0f ? 100.0f * mReduceKeptArea / mReduceFullArea : 0.0f, mReduceBestArea);
		ImGui::Separator();

		const size_t expected = glm::min<size_t>(full.size(), 4);
		passFail(kept.size() == expected, "keeps min(N, 4) points");

		bool allFromInput = true;
		bool noDuplicates = true;
		for (size_t k = 0; k < mReducedIndex.size(); k++)
		{
			if (mReducedIndex[k] < 0) allFromInput = false;
			for (size_t m = k + 1; m < mReducedIndex.size(); m++)
			{
				if (mReducedIndex[k] == mReducedIndex[m]) noDuplicates = false;
			}
		}
		passFail(allFromInput, "every kept point is one of the input points");
		passFail(noDuplicates, "no point kept twice");

		float maxDepth = -FLT_MAX;
		for (const ContactPoint &c: full) maxDepth = glm::max(maxDepth, c.depth);
		bool keepsDeepest = full.empty();
		for (const ContactPoint &c: kept)
		{
			if (glm::abs(c.depth - maxDepth) < 0.0001f) keepsDeepest = true;
		}
		passFail(keepsDeepest, "keeps the deepest point");
		passFail(glm::length(mReduced.normal - mReduceFull.normal) < 0.0001f, "normal unchanged");

		// Kept polygon must not cross itself: every corner turns the same way around the normal.
		bool convex = true;
		if (kept.size() == 4)
		{
			float firstSign = 0.0f;
			for (size_t i = 0; i < 4; i++)
			{
				const glm::vec3 &p0 = kept[i].position;
				const glm::vec3 &p1 = kept[(i + 1) % 4].position;
				const glm::vec3 &p2 = kept[(i + 2) % 4].position;
				const float turn = glm::dot(glm::cross(p1 - p0, p2 - p1), mReduced.normal);
				if (i == 0) firstSign = turn;
				else if (turn * firstSign <= 0.0f) convex = false;
			}
		}
		passFail(convex, "kept points come back in polygon order (convex quad)");
		passFail(mReduceKeptArea >= 0.9f * mReduceBestArea, "kept area >= 90% of the best possible 4");
	}

	void CollisionSandboxDemo::DrawFacesMode(DebugRenderer &debugRenderer)
	{
		if (mFacesValid == false)
		{
			return;
		}

		auto drawFace = [&](const BoxFace &face, const glm::vec4 &color)
		{
			glm::vec3 middle(0.0f);
			for (int i = 0; i < 4; i++)
			{
				debugRenderer.DrawLine(face.vertices[i], face.vertices[(i + 1) % 4], color, 1.0f, true);
				// Vertex 0 is the biggest sphere and they shrink in order, which shows the winding.
				debugRenderer.DrawSphere(face.vertices[i], mPointRadius * (1.6f - 0.35f * static_cast<float>(i)),
				                         color);
				middle += face.vertices[i] * 0.25f;
			}

			if (mDrawNormals)
			{
				debugRenderer.DrawLine(middle, middle + face.normal * mNormalLength, color, 0.6f, true);
				debugRenderer.DrawSphere(middle + face.normal * mNormalLength, mPointRadius * 0.6f, color);
			}
		};

		drawFace(mFaces.reference, kColorReference);
		drawFace(mFaces.incident, kColorIncident);
	}

	void CollisionSandboxDemo::ImGuiFacesResult()
	{
		const ImVec4 green(0.3f, 1.0f, 0.3f, 1.0f);
		const ImVec4 red(1.0f, 0.3f, 0.3f, 1.0f);
		const ImVec4 grey(0.6f, 0.6f, 0.6f, 1.0f);

		if (mSat.overlapping == false)
		{
			ImGui::TextColored(green, "not overlapping: nothing to select");
			return;
		}

		const char *typeNames[] = {"FaceA", "FaceB", "EdgeEdge"};
		ImGui::Text("SAT axis: %s  (indexA %d, indexB %d)", typeNames[static_cast<int>(mSat.type)], mSat.indexA,
		            mSat.indexB);

		if (mFacesValid == false)
		{
			ImGui::Text("An edge pair won, so there is no reference face (that is step 6).");
			return;
		}

		ImGui::TextColored(ImVec4(kColorReference.r, kColorReference.g, kColorReference.b, 1.0f),
		                   "Yellow: reference face   axis %d, sign %+.0f", mFaces.reference.axis,
		                   mFaces.reference.sign);
		ImGui::TextColored(ImVec4(kColorIncident.r, kColorIncident.g, kColorIncident.b, 1.0f),
		                   "Pink: incident face      axis %d, sign %+.0f", mFaces.incident.axis,
		                   mFaces.incident.sign);
		ImGui::Text("flipped: %s", mFaces.flipped ? "true (reference is on the second box)" : "false");
		ImGui::Text("The biggest sphere is vertex 0, then they shrink in order.");
		ImGui::Separator();

		for (const FaceCheck &check: mFaceChecks)
		{
			if (check.state == ECheckState::Skipped)
			{
				ImGui::TextColored(grey, "SKIP  %s (the two calls tie on different axes)", check.label.c_str());
			}
			else
			{
				const bool pass = check.state == ECheckState::Pass;
				ImGui::TextColored(pass ? green : red, "%s  %s", pass ? "PASS" : "FAIL", check.label.c_str());
			}
		}
	}

	void CollisionSandboxDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			SetFlyMode(!mFlyMode);
		}
	}

	void CollisionSandboxDemo::DrawBox(DebugRenderer &debugRenderer, const SandboxBox &box, glm::vec4 color) const
	{
		if (mDrawSolid)
		{
			debugRenderer.DrawBox(box.position, glm::radians(box.eulerDegrees), box.size, color);
		}

		const OBB obb = box.ToOBB();
		const glm::vec3 h = obb.halfExtents;

		glm::vec3 corners[8];
		for (int i = 0; i < 8; i++)
		{
			glm::vec3 local((i & 1) ? h.x : -h.x, (i & 2) ? h.y : -h.y, (i & 4) ? h.z : -h.z);
			corners[i] = obb.GetPointInWorldSpace(local);
		}

		// Two corners share an edge when their indices differ in exactly one bit.
		const glm::vec4 edgeColor(color.r, color.g, color.b, 1.0f);
		for (int i = 0; i < 8; i++)
		{
			for (int bit = 1; bit <= 4; bit <<= 1)
			{
				if ((i & bit) == 0)
				{
					debugRenderer.DrawLine(corners[i], corners[i | bit], edgeColor, 0.3f);
				}
			}
		}
	}

	void CollisionSandboxDemo::RunClip()
	{
		glm::vec3 centre(0.0f);
		for (const glm::vec3 &v: mClipInput) centre += v;
		if (mClipInput.empty() == false) centre /= static_cast<float>(mClipInput.size());

		// The plane normal stays in the XY plane so the whole thing reads as a 2D picture.
		const float angle = glm::radians(mClipAngleDegrees);
		mClipPlane.normal = glm::vec3(cos(angle), sin(angle), 0.0f);
		mClipPlane.point = centre + mClipPlane.normal * mClipOffset;

		mClipOutput = ClipPolygonAgainstPlane(mClipInput, mClipPlane);

		// Checks that do not re-implement the algorithm:
		//  - every output point is on the keep side (distance <= 0)
		//  - output size == kept input vertices + edges that cross the plane
		auto distance = [this](const glm::vec3 &p)
		{
			return glm::dot(p - mClipPlane.point, mClipPlane.normal);
		};

		mClipCheck = ClipCheck{};
		const size_t count = mClipInput.size();
		for (size_t i = 0; i < count; i++)
		{
			const bool fromIn = distance(mClipInput[i]) <= 0.0f;
			const bool toIn = distance(mClipInput[(i + 1) % count]) <= 0.0f;
			if (fromIn) mClipCheck.expectedCount++;
			if (fromIn != toIn) mClipCheck.expectedCount++;
		}

		mClipCheck.maxOutsideDistance = -FLT_MAX;
		for (const glm::vec3 &p: mClipOutput)
		{
			mClipCheck.maxOutsideDistance = glm::max(mClipCheck.maxOutsideDistance, distance(p));
		}
		if (mClipOutput.empty()) mClipCheck.maxOutsideDistance = 0.0f;

		mClipCheck.mainPass = static_cast<int>(mClipOutput.size()) == mClipCheck.expectedCount &&
		                      mClipCheck.maxOutsideDistance <= 0.001f;

		// Edge cases, run every frame with the current polygon and plane direction.
		mClipCheck.emptyPass = ClipPolygonAgainstPlane({}, mClipPlane).empty();

		ClipPlane farPlane = mClipPlane;
		farPlane.point = centre + mClipPlane.normal * 100000.0f;
		mClipCheck.allInPass = ClipPolygonAgainstPlane(mClipInput, farPlane).size() == count;

		farPlane.point = centre - mClipPlane.normal * 100000.0f;
		mClipCheck.allOutPass = ClipPolygonAgainstPlane(mClipInput, farPlane).empty();
	}

	void CollisionSandboxDemo::DrawClipMode(DebugRenderer &debugRenderer)
	{
		RunClip();

		const glm::vec4 inputColor(0.6f, 0.6f, 0.6f, 1.0f);
		const glm::vec4 planeColor(0.95f, 0.45f, 0.15f, 1.0f);
		const glm::vec4 outputColor(0.1f, 0.9f, 0.5f, 1.0f);

		for (size_t i = 0; i < mClipInput.size(); i++)
		{
			debugRenderer.DrawLine(mClipInput[i], mClipInput[(i + 1) % mClipInput.size()], inputColor, 0.3f);
			debugRenderer.DrawSphere(mClipInput[i], mPointRadius * 0.8f, inputColor);
		}

		// The plane, seen edge-on, plus a short arrow along its normal (the discarded side).
		const glm::vec3 tangent(-mClipPlane.normal.y, mClipPlane.normal.x, 0.0f);
		debugRenderer.DrawLine(mClipPlane.point - tangent * 90.0f, mClipPlane.point + tangent * 90.0f, planeColor,
		                       0.4f, true);
		debugRenderer.DrawLine(mClipPlane.point, mClipPlane.point + mClipPlane.normal * 12.0f, planeColor, 0.8f,
		                       true);

		for (size_t i = 0; i < mClipOutput.size(); i++)
		{
			debugRenderer.DrawLine(mClipOutput[i], mClipOutput[(i + 1) % mClipOutput.size()], outputColor, 0.8f,
			                       true);
			debugRenderer.DrawSphere(mClipOutput[i], mPointRadius, kColorPoint);
		}
	}

	void CollisionSandboxDemo::DebugDraw(DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);

		if (mMode == EMode::ClipPolygon)
		{
			DrawClipMode(debugRenderer);
			return;
		}

		RunCollision();

		DrawBox(debugRenderer, mBoxA, kColorA);
		DrawBox(debugRenderer, mBoxB, kColorB);

		if (mMode == EMode::Faces)
		{
			DrawFacesMode(debugRenderer);
			return;
		}

		if (mMode == EMode::ClipSteps)
		{
			DrawClipStepsMode(debugRenderer);
			return;
		}

		if (mMode == EMode::EdgeEdge)
		{
			DrawEdgeEdgeMode(debugRenderer);
			return;
		}

		if (mMode == EMode::Reduce)
		{
			DrawReduceMode(debugRenderer);
			return;
		}

		if (IsSatMode())
		{
			// The normal is drawn from the centre of the first box passed to BoxBoxSAT,
			// so it should always point towards the other box.
			if (mSat.overlapping && mDrawNormals)
			{
				const glm::vec3 from = mSwapOrder ? mBoxB.position : mBoxA.position;
				debugRenderer.DrawLine(from, from + mSat.normal * mNormalLength, kColorNormal, 0.6f, true);
				debugRenderer.DrawSphere(from + mSat.normal * mNormalLength, mPointRadius, kColorNormal);
			}
			return;
		}

		if (mResult.colliding == false)
		{
			return;
		}

		for (const glm::vec3 &point: mResult.points)
		{
			if (mDrawNormals)
			{
				debugRenderer.DrawLine(point, point + mResult.normal * mNormalLength, kColorNormal, 0.4f, true);
			}
			debugRenderer.DrawSphere(point, mPointRadius, kColorPoint);
		}
	}

	void CollisionSandboxDemo::CopyPoseToClipboard() const
	{
		char buffer[512];
		snprintf(buffer, sizeof(buffer),
		         "A pos(%.3f, %.3f, %.3f) rotDeg(%.3f, %.3f, %.3f) size(%.3f, %.3f, %.3f)\n"
		         "B pos(%.3f, %.3f, %.3f) rotDeg(%.3f, %.3f, %.3f) size(%.3f, %.3f, %.3f)\n"
		         "order %s | colliding %d | points %d | penetration %.4f | normal(%.3f, %.3f, %.3f)\n",
		         mBoxA.position.x, mBoxA.position.y, mBoxA.position.z,
		         mBoxA.eulerDegrees.x, mBoxA.eulerDegrees.y, mBoxA.eulerDegrees.z,
		         mBoxA.size.x, mBoxA.size.y, mBoxA.size.z,
		         mBoxB.position.x, mBoxB.position.y, mBoxB.position.z,
		         mBoxB.eulerDegrees.x, mBoxB.eulerDegrees.y, mBoxB.eulerDegrees.z,
		         mBoxB.size.x, mBoxB.size.y, mBoxB.size.z,
		         mSwapOrder ? "(B, A)" : "(A, B)", mResult.colliding ? 1 : 0,
		         static_cast<int>(mResult.points.size()), mResult.penetration,
		         mResult.normal.x, mResult.normal.y, mResult.normal.z);

		std::string text(buffer);
		for (size_t i = 0; i < mResult.points.size(); i++)
		{
			const glm::vec3 &p = mResult.points[i];
			snprintf(buffer, sizeof(buffer), "  p%d (%.3f, %.3f, %.3f)\n", static_cast<int>(i), p.x, p.y, p.z);
			text += buffer;
		}

		ImGui::SetClipboardText(text.c_str());
		printf("%s", text.c_str());
	}

	void CollisionSandboxDemo::ImGuiProc()
	{
		EkScene::ImGuiProc();

		ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Collision Sandbox", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("TAB: switch between editing and fly camera (%s)", mFlyMode ? "FLY" : "EDIT");
			ImGui::Text("ESC: back to demo list");
			ImGui::Separator();

			int mode = static_cast<int>(mMode);
			ImGui::RadioButton("Step 1: clip polygon", &mode, static_cast<int>(EMode::ClipPolygon));
			ImGui::SameLine();
			ImGui::RadioButton("Step 2: SAT", &mode, static_cast<int>(EMode::Sat));
			ImGui::SameLine();
			ImGui::RadioButton("SAT tests", &mode, static_cast<int>(EMode::SatTests));
			ImGui::SameLine();
			ImGui::RadioButton("Step 3: faces", &mode, static_cast<int>(EMode::Faces));
			ImGui::SameLine();
			ImGui::RadioButton("Step 4: clip steps", &mode, static_cast<int>(EMode::ClipSteps));
			ImGui::SameLine();
			ImGui::RadioButton("Step 6: edge-edge", &mode, static_cast<int>(EMode::EdgeEdge));
			ImGui::SameLine();
			ImGui::RadioButton("Step 7: reduce", &mode, static_cast<int>(EMode::Reduce));
			ImGui::SameLine();
			ImGui::RadioButton("BoxVsBox", &mode, static_cast<int>(EMode::BoxVsBox));
			if (mode != static_cast<int>(mMode) && mode == static_cast<int>(EMode::EdgeEdge))
			{
				ApplyPreset(4); // start on the crossing edges
				mDrawSolid = false;
			}
			if (mode != static_cast<int>(mMode) && mode == static_cast<int>(EMode::SatTests))
			{
				SelectSatCase(mSelectedSatCase);
			}
			if (mode != static_cast<int>(mMode) && mode == static_cast<int>(EMode::ClipSteps))
			{
				ApplyPreset(6); // start on the case that overhangs every edge
				mDrawSolid = false;
			}
			if (mode != static_cast<int>(mMode) && mode == static_cast<int>(EMode::Reduce))
			{
				ApplyPreset(7); // start on the tilted octagon
				mDrawSolid = false;
			}
			mMode = static_cast<EMode>(mode);
			ImGui::Separator();

			if (mMode == EMode::ClipPolygon)
			{
				ImGuiClipMode();
			}
			else if (mMode == EMode::SatTests)
			{
				ImGuiSatTestsMode();
			}
			else
			{
				ImGuiBoxMode();
			}
		}
		ImGui::End();
	}

	void CollisionSandboxDemo::ImGuiClipMode()
	{
		auto passFail = [](const char *label, bool pass)
		{
			ImGui::TextColored(pass ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) : ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s  %s",
			                   pass ? "PASS" : "FAIL", label);
		};

		ImGui::Text("Grey: input polygon. Orange: clip plane.");
		ImGui::Text("The orange arrow points to the discarded side.");
		ImGui::Text("Green: what ClipPolygonAgainstPlane returned.");
		ImGui::Separator();

		ImGui::SliderFloat("Plane angle (deg)", &mClipAngleDegrees, -180.0f, 180.0f);
		ImGui::SliderFloat("Plane offset", &mClipOffset, -80.0f, 80.0f);
		for (size_t i = 0; i < mClipInput.size(); i++)
		{
			char label[16];
			snprintf(label, sizeof(label), "v%d", static_cast<int>(i));
			ImGui::DragFloat3(label, &mClipInput[i].x, 0.2f);
		}
		ImGui::SliderFloat("Point radius", &mPointRadius, 0.2f, 5.0f);
		ImGui::Separator();

		ImGui::Text("Output points: %d (expected %d)", static_cast<int>(mClipOutput.size()),
		            mClipCheck.expectedCount);
		for (size_t i = 0; i < mClipOutput.size(); i++)
		{
			const glm::vec3 &p = mClipOutput[i];
			ImGui::Text("  out%d (%.2f, %.2f, %.2f)", static_cast<int>(i), p.x, p.y, p.z);
		}
		ImGui::Separator();

		passFail("count matches and all points on keep side", mClipCheck.mainPass);
		passFail("empty polygon gives empty output", mClipCheck.emptyPass);
		passFail("plane far away, all in: output equals input size", mClipCheck.allInPass);
		passFail("plane far away, all out: output is empty", mClipCheck.allOutPass);
	}

	void CollisionSandboxDemo::SelectSatCase(int index)
	{
		if (index < 0 || index >= static_cast<int>(mSatCases.size()))
		{
			return;
		}

		mSelectedSatCase = index;
		mSwapOrder = false;

		const SatTestCase &testCase = mSatCases[index];
		mBoxA = SandboxBox{testCase.a.position, testCase.a.eulerDegrees, testCase.a.size};
		mBoxB = SandboxBox{testCase.b.position, testCase.b.eulerDegrees, testCase.b.size};
	}

	void CollisionSandboxDemo::ImGuiSatTestsMode()
	{
		const ImVec4 green(0.3f, 1.0f, 0.3f, 1.0f);
		const ImVec4 red(1.0f, 0.3f, 0.3f, 1.0f);
		const char *typeNames[] = {"FaceA", "FaceB", "EdgeEdge"};

		// Cheap enough to run every frame, so the list is always up to date.
		std::vector<SatTestOutcome> outcomes;
		int passed = 0;
		for (const SatTestCase &testCase: mSatCases)
		{
			outcomes.push_back(RunSatTest(testCase));
			if (outcomes.back().pass) passed++;
		}

		const int total = static_cast<int>(mSatCases.size());
		ImGui::TextColored(passed == total ? green : red, "%d / %d passed", passed, total);
		ImGui::Text("Click a case to show it. The boxes on screen are that case.");
		ImGui::Separator();

		ImGui::BeginChild("SatCases", ImVec2(520.0f, 360.0f), true);
		for (int i = 0; i < total; i++)
		{
			ImGui::PushID(i);
			ImGui::TextColored(outcomes[i].pass ? green : red, outcomes[i].pass ? "PASS" : "FAIL");
			ImGui::SameLine();
			if (ImGui::Selectable(mSatCases[i].name.c_str(), mSelectedSatCase == i))
			{
				SelectSatCase(i);
			}
			if (outcomes[i].pass == false)
			{
				ImGui::Indent();
				ImGui::TextColored(red, "%s", outcomes[i].failures.c_str());
				ImGui::Unindent();
			}
			ImGui::PopID();
		}
		ImGui::EndChild();

		if (mSelectedSatCase < total)
		{
			const SatTestCase &testCase = mSatCases[mSelectedSatCase];
			const SatResult &result = outcomes[mSelectedSatCase].result;

			ImGui::Separator();
			ImGui::Text("%s", testCase.name.c_str());

			if (testCase.expectOverlap)
			{
				ImGui::Text("Expected: overlapping, penetration %.3f", testCase.penetration);
				if (glm::length(testCase.normal) > 0.5f)
				{
					ImGui::Text("          normal (%.3f, %.3f, %.3f)", testCase.normal.x, testCase.normal.y,
					            testCase.normal.z);
				}
				if (testCase.checkAxis)
				{
					ImGui::Text("          axis %s (indexA %d, indexB %d)", typeNames[static_cast<int>(testCase.type)],
					            testCase.indexA, testCase.indexB);
				}
				else if (testCase.expectAnyFace)
				{
					ImGui::Text("          axis FaceA or FaceB, not EdgeEdge");
				}
			}
			else
			{
				ImGui::Text("Expected: not overlapping");
			}

			if (result.overlapping)
			{
				ImGui::Text("Got:      overlapping, penetration %.3f", result.penetration);
				ImGui::Text("          normal (%.3f, %.3f, %.3f)", result.normal.x, result.normal.y, result.normal.z);
				ImGui::Text("          axis %s (indexA %d, indexB %d)", typeNames[static_cast<int>(result.type)],
				            result.indexA, result.indexB);
			}
			else
			{
				ImGui::Text("Got:      not overlapping");
			}
		}

		ImGui::Separator();
		ImGui::Checkbox("Solid boxes", &mDrawSolid);
		ImGui::SameLine();
		ImGui::Checkbox("Normals", &mDrawNormals);
		ImGui::SliderFloat("Normal length", &mNormalLength, 1.0f, 50.0f);
	}

	void CollisionSandboxDemo::ImGuiBoxMode()
	{
		{
			ImGui::Text("Presets");
			for (int i = 0; i < IM_ARRAYSIZE(kPresets); i++)
			{
				if (ImGui::Button(kPresets[i], ImVec2(-1.0f, 0.0f)))
				{
					ApplyPreset(i);
				}
			}
			ImGui::Separator();

			ImGui::TextColored(ImVec4(kColorA.r, kColorA.g, kColorA.b, 1.0f), "Box A");
			ImGui::DragFloat3("A position", &mBoxA.position.x, 0.1f);
			ImGui::DragFloat3("A rotation (deg)", &mBoxA.eulerDegrees.x, 0.25f);
			ImGui::DragFloat3("A size", &mBoxA.size.x, 0.1f, 1.0f, 500.0f);

			ImGui::TextColored(ImVec4(kColorB.r, kColorB.g, kColorB.b, 1.0f), "Box B");
			ImGui::DragFloat3("B position", &mBoxB.position.x, 0.1f);
			ImGui::DragFloat3("B rotation (deg)", &mBoxB.eulerDegrees.x, 0.25f);
			ImGui::DragFloat3("B size", &mBoxB.size.x, 0.1f, 1.0f, 500.0f);
			ImGui::Text("Hold ALT while dragging for fine steps, SHIFT for coarse.");
			ImGui::Separator();

			ImGui::Checkbox("Swap order: call as (B, A)", &mSwapOrder);
			ImGui::Checkbox("Solid boxes", &mDrawSolid);
			ImGui::SameLine();
			ImGui::Checkbox("Normals", &mDrawNormals);
			ImGui::SliderFloat("Point radius", &mPointRadius, 0.2f, 5.0f);
			ImGui::SliderFloat("Normal length", &mNormalLength, 1.0f, 50.0f);
			ImGui::Separator();

			ImGui::Text("Result");
			if (mMode == EMode::Faces)
			{
				ImGuiFacesResult();
				return;
			}
			if (mMode == EMode::ClipSteps)
			{
				ImGuiClipStepsResult();
				return;
			}
			if (mMode == EMode::EdgeEdge)
			{
				ImGuiEdgeEdgeResult();
				return;
			}
			if (mMode == EMode::Reduce)
			{
				ImGuiReduceResult();
				return;
			}
			if (mMode == EMode::Sat)
			{
				if (mSat.overlapping)
				{
					const char *typeNames[] = {"FaceA", "FaceB", "EdgeEdge"};
					ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "OVERLAPPING");
					ImGui::Text("Axis: %s  (indexA %d, indexB %d)", typeNames[static_cast<int>(mSat.type)],
					            mSat.indexA, mSat.indexB);
					ImGui::Text("Penetration: %.4f", mSat.penetration);
					ImGui::Text("Normal: (%.3f, %.3f, %.3f)", mSat.normal.x, mSat.normal.y, mSat.normal.z);
				}
				else
				{
					ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "not overlapping");
				}
				ImGui::TextColored(mSatSymmetric ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) : ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
				                   "%s  (A, B) and (B, A) agree, normals opposite", mSatSymmetric ? "PASS" : "FAIL");
				return;
			}

			if (mResult.colliding)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "COLLIDING");
				ImGui::Text("Points: %d", static_cast<int>(mResult.points.size()));
				ImGui::Text("Penetration: %.4f", mResult.penetration);
				ImGui::Text("Normal: (%.3f, %.3f, %.3f)", mResult.normal.x, mResult.normal.y, mResult.normal.z);
				for (size_t i = 0; i < mResult.points.size(); i++)
				{
					const glm::vec3 &p = mResult.points[i];
					ImGui::Text("  p%d (%.2f, %.2f, %.2f)", static_cast<int>(i), p.x, p.y, p.z);
				}
			}
			else
			{
				ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "not colliding");
			}

			if (ImGui::Button("Copy pose + result", ImVec2(-1.0f, 0.0f)))
			{
				CopyPoseToClipboard();
			}
		}
	}
} // namespace Eklavya
