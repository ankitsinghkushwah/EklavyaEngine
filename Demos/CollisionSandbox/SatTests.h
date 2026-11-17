//
// TEMPORARY TOOL: test cases for BoxBoxSAT (step 2 of the box-box contact rewrite).
// Depends only on glm and EkPhysics/CollisionSystem.h.
//

#ifndef INCSATTESTS_H
#define INCSATTESTS_H

#include <EkPhysics/CollisionSystem.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Eklavya
{
	struct SatTestBox
	{
		glm::vec3 position{0.0f};
		glm::vec3 eulerDegrees{0.0f};
		glm::vec3 size{40.0f};

		Physics::OBB ToOBB() const;
	};

	struct SatTestCase
	{
		std::string name;
		SatTestBox a;
		SatTestBox b;

		bool expectOverlap = true;
		float penetration = 0.0f;
		glm::vec3 normal{0.0f}; // expected, pointing from A to B

		// Which axis should win. Left unchecked where two axes tie or the pose is degenerate.
		bool checkAxis = true;
		Physics::ESatAxis type = Physics::ESatAxis::FaceA;
		int indexA = -1;
		int indexB = -1;

		// For poses where a face of A and a face of B tie: either may win, an edge pair may not.
		bool expectAnyFace = false;
	};

	struct SatTestOutcome
	{
		bool pass = true;
		Physics::SatResult result;
		std::string failures; // one line per failed check, empty when everything passed
	};

	std::vector<SatTestCase> BuildSatTestCases();

	// Runs every check on one case:
	//  - overlap flag, penetration, normal and (optionally) winning axis match the expectation
	//  - normal is unit length and points from A to B
	//  - calling with (B, A) gives the same overlap and penetration with the normal reversed
	//  - pushing B along the normal by slightly more than the penetration separates the boxes,
	//    and by slightly less does not (the result really is the minimum translation)
	SatTestOutcome RunSatTest(const SatTestCase &testCase);
} // namespace Eklavya

#endif
