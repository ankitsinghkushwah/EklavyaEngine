//
// TEMPORARY TOOL: test cases for BoxBoxSAT. See the header.
//

#include "SatTests.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <cstdarg>
#include <cstdio>

using namespace Eklavya::Physics;

namespace Eklavya
{
	namespace
	{
		const float kTolerance = 0.01f; // boxes are ~40 units, so this is tight
		const float kRoot2 = 1.41421356f;

		const char *AxisName(ESatAxis type)
		{
			switch (type)
			{
				case ESatAxis::FaceA: return "FaceA";
				case ESatAxis::FaceB: return "FaceB";
				default: return "EdgeEdge";
			}
		}

		SatTestBox Box(glm::vec3 position, glm::vec3 eulerDegrees = glm::vec3(0.0f), glm::vec3 size = glm::vec3(40.0f))
		{
			SatTestBox box;
			box.position = position;
			box.eulerDegrees = eulerDegrees;
			box.size = size;
			return box;
		}

		// Rotates the whole scene (both boxes and the expected normal) about the origin.
		// The answer must not depend on how the pair is oriented in the world, and nothing
		// lines up with the world axes any more, so floating point ties get exercised too.
		SatTestCase Tilted(const SatTestCase &source, glm::vec3 tiltDegrees)
		{
			const glm::quat tilt(glm::radians(tiltDegrees));

			auto tiltBox = [&tilt](const SatTestBox &box)
			{
				SatTestBox out = box;
				out.position = tilt * box.position;
				out.eulerDegrees = glm::degrees(glm::eulerAngles(tilt * glm::quat(glm::radians(box.eulerDegrees))));
				return out;
			};

			SatTestCase out = source;
			out.name = source.name + ", whole scene tilted";
			out.a = tiltBox(source.a);
			out.b = tiltBox(source.b);
			out.normal = tilt * source.normal;
			return out;
		}

		void Fail(SatTestOutcome &outcome, const char *format, ...)
		{
			char buffer[256];
			va_list args;
			va_start(args, format);
			vsnprintf(buffer, sizeof(buffer), format, args);
			va_end(args);

			outcome.pass = false;
			outcome.failures += buffer;
			outcome.failures += "\n";
		}
	} // namespace

	OBB SatTestBox::ToOBB() const
	{
		OBB obb;
		obb.center = position;
		obb.halfExtents = size * 0.5f;
		obb.rotation = glm::toMat3(glm::quat(glm::radians(eulerDegrees)));
		obb.worldTransform = glm::translate(glm::mat4(1.0f), position) * glm::mat4(obb.rotation);
		return obb;
	}

	std::vector<SatTestCase> BuildSatTestCases()
	{
		std::vector<SatTestCase> cases;

		// Box A is a 40 unit cube centred at (0, 40, 0) unless a case says otherwise.
		const glm::vec3 centreA(0.0f, 40.0f, 0.0f);
		const glm::vec3 tilt(25.0f, 40.0f, 15.0f);

		// ---- Face of A wins: B pushed 2 units into each of A's six faces -------------------
		const char *faceNames[6] = {"+x", "-x", "+y", "-y", "+z", "-z"};
		for (int face = 0; face < 6; face++)
		{
			glm::vec3 direction(0.0f);
			direction[face / 2] = (face % 2 == 0) ? 1.0f : -1.0f;

			SatTestCase c;
			c.name = std::string("Aligned, B on A's ") + faceNames[face] + " face";
			c.a = Box(centreA);
			c.b = Box(centreA + direction * 38.0f);
			c.penetration = 2.0f;
			c.normal = direction;
			c.type = ESatAxis::FaceA; // A's and B's face axes tie here; A's are tested first
			c.indexA = face / 2;
			cases.push_back(c);
		}

		// ---- Different sizes ---------------------------------------------------------------
		{
			SatTestCase c;
			c.name = "Small box on a wide slab";
			c.a = Box(centreA, glm::vec3(0.0f), glm::vec3(100.0f, 40.0f, 100.0f));
			c.b = Box(glm::vec3(30.0f, 64.0f, -20.0f), glm::vec3(0.0f), glm::vec3(10.0f));
			c.penetration = 1.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Non-cube boxes side by side";
			c.a = Box(centreA, glm::vec3(0.0f), glm::vec3(20.0f, 60.0f, 30.0f));
			c.b = Box(centreA + glm::vec3(-24.0f, 5.0f, 3.0f), glm::vec3(0.0f), glm::vec3(30.0f, 10.0f, 80.0f));
			c.penetration = 1.0f; // half widths on x: 10 + 15 = 25, centres 24 apart
			c.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
			c.indexA = 0;
			cases.push_back(c);
		}

		// ---- Rotated B, still a face of A --------------------------------------------------
		{
			SatTestCase c;
			c.name = "B yawed 45 on top of A";
			c.a = Box(centreA);
			c.b = Box(glm::vec3(0.0f, 78.0f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));
			// A's and B's y faces tie with an edge pair here. Rounding may pick either face,
			// but it must not pick the edge pair.
			cases.back().checkAxis = false;
			cases.back().expectAnyFace = true;
		}
		{
			SatTestCase c;
			c.name = "B's edge resting on A's top face";
			c.a = Box(centreA);
			c.b = Box(glm::vec3(0.0f, 60.0f + 20.0f * kRoot2 - 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));
		}
		{
			SatTestCase c;
			c.name = "B's corner resting on A's top face";
			c.a = Box(centreA);
			// Rotated so B's body diagonal is vertical; the corner is 20 * sqrt(3) below B's centre.
			c.b = Box(glm::vec3(0.0f, 60.0f + 20.0f * 1.7320508f - 2.0f, 0.0f), glm::vec3(-45.0f, 0.0f, 35.264f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));
		}

		// ---- Face of B wins ----------------------------------------------------------------
		{
			SatTestCase c;
			c.name = "A's edge poking into B's bottom face";
			c.a = Box(centreA, glm::vec3(0.0f, 0.0f, 45.0f)); // A stands on an edge, top edge at y = 40 + 20*sqrt(2)
			c.b = Box(glm::vec3(0.0f, 40.0f + 20.0f * kRoot2 + 20.0f - 2.0f, 0.0f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.type = ESatAxis::FaceB;
			c.indexB = 1;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));
		}
		{
			SatTestCase c;
			c.name = "A's edge poking into B's side face";
			c.a = Box(centreA, glm::vec3(0.0f, 45.0f, 0.0f));
			c.b = Box(centreA + glm::vec3(-(20.0f * kRoot2 + 20.0f - 3.0f), 0.0f, 0.0f));
			c.penetration = 3.0f;
			c.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
			c.type = ESatAxis::FaceB;
			c.indexB = 0;
			cases.push_back(c);
		}

		// ---- Edge against edge -------------------------------------------------------------
		{
			SatTestCase c;
			c.name = "Edges crossing at right angles";
			// A's top edge runs along z, B's bottom edge runs along x.
			c.a = Box(centreA, glm::vec3(0.0f, 0.0f, 45.0f));
			c.b = Box(glm::vec3(0.0f, 40.0f + 40.0f * kRoot2 - 2.0f, 0.0f), glm::vec3(45.0f, 0.0f, 0.0f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.type = ESatAxis::EdgeEdge;
			c.indexA = 2;
			c.indexB = 0;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));

			// Same pose with B below A: the normal must flip.
			SatTestCase below = c;
			below.name = "Edges crossing, B underneath A";
			below.b.position.y = 40.0f - 40.0f * kRoot2 + 2.0f;
			below.normal = glm::vec3(0.0f, -1.0f, 0.0f);
			cases.push_back(below);
		}

		// ---- Not overlapping ---------------------------------------------------------------
		{
			SatTestCase c;
			c.name = "Separated along a face axis";
			c.a = Box(centreA);
			c.b = Box(glm::vec3(0.0f, 90.0f, 0.0f));
			c.expectOverlap = false;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Just apart (gap of 0.05)";
			c.a = Box(centreA);
			c.b = Box(centreA + glm::vec3(40.05f, 0.0f, 0.0f));
			c.expectOverlap = false;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Separated diagonally, rotated B";
			c.a = Box(centreA);
			c.b = Box(centreA + glm::vec3(55.0f, 55.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
			c.expectOverlap = false;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Only an edge axis separates (all 6 face axes overlap)";
			// The crossing-edges pose lifted to leave a gap of 1 between the two edges.
			c.a = Box(centreA, glm::vec3(0.0f, 0.0f, 45.0f));
			c.b = Box(glm::vec3(0.0f, 40.0f + 40.0f * kRoot2 + 1.0f, 0.0f), glm::vec3(45.0f, 0.0f, 0.0f));
			c.expectOverlap = false;
			cases.push_back(c);
			cases.push_back(Tilted(c, tilt));
		}

		// ---- Degenerate poses: must not produce NaN or a zero normal ------------------------
		{
			SatTestCase c;
			c.name = "Nearly parallel edges (B yawed 0.1 deg)";
			c.a = Box(centreA);
			c.b = Box(glm::vec3(0.0f, 78.0f, 0.0f), glm::vec3(0.0f, 0.1f, 0.0f));
			c.penetration = 2.0f;
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Deep overlap, B mostly inside A";
			c.a = Box(centreA, glm::vec3(0.0f), glm::vec3(100.0f));
			c.b = Box(centreA + glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(0.0f), glm::vec3(20.0f));
			c.penetration = 15.0f; // 50 + 10 - 45
			c.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			c.indexA = 1;
			cases.push_back(c);
		}
		{
			SatTestCase c;
			c.name = "Same centre (direction is arbitrary)";
			c.a = Box(centreA);
			c.b = Box(centreA, glm::vec3(0.0f), glm::vec3(30.0f));
			c.penetration = 35.0f; // 20 + 15 on every face axis
			c.checkAxis = false;
			c.normal = glm::vec3(0.0f); // not checked: any face direction is a valid answer
			cases.push_back(c);
		}

		return cases;
	}

	SatTestOutcome RunSatTest(const SatTestCase &testCase)
	{
		SatTestOutcome outcome;

		const OBB a = testCase.a.ToOBB();
		const OBB b = testCase.b.ToOBB();

		const SatResult result = BoxBoxSAT(a, b);
		const SatResult swapped = BoxBoxSAT(b, a);
		outcome.result = result;

		if (result.overlapping != testCase.expectOverlap)
		{
			Fail(outcome, "overlapping: got %s, expected %s", result.overlapping ? "true" : "false",
			     testCase.expectOverlap ? "true" : "false");
			return outcome;
		}

		if (swapped.overlapping != result.overlapping)
		{
			Fail(outcome, "(B, A) disagrees with (A, B) about overlap");
		}

		if (result.overlapping == false)
		{
			return outcome;
		}

		// Expected values
		if (std::abs(result.penetration - testCase.penetration) > kTolerance)
		{
			Fail(outcome, "penetration: got %.4f, expected %.4f", result.penetration, testCase.penetration);
		}

		const bool normalIsFinite = std::isfinite(result.normal.x) && std::isfinite(result.normal.y) &&
		                            std::isfinite(result.normal.z);
		if (normalIsFinite == false)
		{
			Fail(outcome, "normal is NaN or infinite");
			return outcome;
		}

		if (std::abs(glm::length(result.normal) - 1.0f) > 0.001f)
		{
			Fail(outcome, "normal is not unit length (%.4f)", glm::length(result.normal));
		}

		const bool expectsDirection = glm::length(testCase.normal) > 0.5f;
		if (expectsDirection && glm::length(result.normal - testCase.normal) > kTolerance)
		{
			Fail(outcome, "normal: got (%.3f, %.3f, %.3f), expected (%.3f, %.3f, %.3f)",
			     result.normal.x, result.normal.y, result.normal.z,
			     testCase.normal.x, testCase.normal.y, testCase.normal.z);
		}

		if (glm::dot(result.normal, b.center - a.center) < -0.001f)
		{
			Fail(outcome, "normal points from B to A");
		}

		if (testCase.checkAxis)
		{
			if (result.type != testCase.type)
			{
				Fail(outcome, "axis type: got %s, expected %s", AxisName(result.type), AxisName(testCase.type));
			}
			else if (result.indexA != testCase.indexA || result.indexB != testCase.indexB)
			{
				Fail(outcome, "axis index: got (A %d, B %d), expected (A %d, B %d)", result.indexA, result.indexB,
				     testCase.indexA, testCase.indexB);
			}
		}

		if (testCase.expectAnyFace && result.type == ESatAxis::EdgeEdge)
		{
			Fail(outcome, "axis type: got EdgeEdge, expected a face (a face and an edge pair tie here)");
		}

		// Swapping the arguments
		if (swapped.overlapping)
		{
			if (std::abs(swapped.penetration - result.penetration) > kTolerance)
			{
				Fail(outcome, "(B, A) penetration differs: %.4f vs %.4f", swapped.penetration, result.penetration);
			}
			if (expectsDirection && glm::length(swapped.normal + result.normal) > kTolerance)
			{
				Fail(outcome, "(B, A) normal is not the reverse of (A, B)");
			}
		}

		// Minimum translation: moving B out along the normal by the penetration must just separate them.
		const float margin = 0.02f;
		SatTestBox moved = testCase.b;
		moved.position = testCase.b.position + result.normal * (result.penetration + margin);
		if (BoxBoxSAT(a, moved.ToOBB()).overlapping)
		{
			Fail(outcome, "still overlapping after moving B out by penetration + %.2f", margin);
		}

		moved.position = testCase.b.position + result.normal * (result.penetration - margin);
		if (BoxBoxSAT(a, moved.ToOBB()).overlapping == false)
		{
			Fail(outcome, "already separated after moving B out by penetration - %.2f", margin);
		}

		return outcome;
	}
} // namespace Eklavya
