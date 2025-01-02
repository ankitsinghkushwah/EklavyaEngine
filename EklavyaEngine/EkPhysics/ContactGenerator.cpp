//
//  ContactGenerator.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#include "ContactGenerator.h"
#include "Collider.h"
#include "Scene/EkActor.h"
#include "Components/EkComponent.hpp"

namespace Eklavya::Physics::ContactGenerator
{

	bool BoxAndSphere(const BoxColliderComponent& box, const SphereColliderComponent& sphere)
	{
		glm::vec3 localSpherePosition = box.GetPointInLocalSpace(sphere.GetPosition());

		bool xIsGreater = abs(localSpherePosition.x) - sphere.GetRadius() > box.GetHalfSize().x;
		bool yIsGreater = abs(localSpherePosition.y) - sphere.GetRadius() > box.GetHalfSize().y;
		bool zIsGreater = abs(localSpherePosition.z) - sphere.GetRadius() > box.GetHalfSize().z;

		if (!xIsGreater && !yIsGreater && !zIsGreater)
			return true;
		return false;
	}

	bool SphereAndSphere(const SphereColliderComponent& one, const SphereColliderComponent& two)
	{
		glm::vec3 midline = one.GetPosition() - two.GetPosition();
		float     totalRadius = one.GetRadius() + two.GetRadius();
		return glm::length2(midline) < totalRadius * totalRadius;
	}

	bool SphereAndSphere(const SphereColliderComponent& sphereOne, const SphereColliderComponent& sphereTwo, CollisionPointInfo& newContact)
	{
		if (!SphereAndSphere(sphereOne, sphereTwo))
			return false;

		glm::vec3 p1 = sphereOne.GetPosition();
		glm::vec3 p2 = sphereTwo.GetPosition();

		glm::vec3 toSphereTwo = p2 - p1;
		float     size = glm::length(toSphereTwo);

		newContact.normal = -1.0f * (toSphereTwo / size);
		newContact.location = p1 + (toSphereTwo / 2.0f);
		newContact.peneteration = (sphereOne.GetRadius() + sphereTwo.GetRadius()) - size;

		return true;
	}

	bool SphereAndBox(const BoxColliderComponent& box, const SphereColliderComponent& sphere, CollisionPointInfo& newContact)
	{
		// early test
		if (!BoxAndSphere(box, sphere))
			return false;

		glm::vec3 relativePos = box.GetPointInLocalSpace(sphere.GetPosition());
		glm::vec3 halfSize = box.GetHalfSize();
		glm::vec3 closestPoint = glm::clamp(relativePos, -1.0f * halfSize, halfSize);

		glm::vec3 closestPointWorld = box.GetPointInWorldSpace(closestPoint);

		float dist = glm::length2(closestPoint - relativePos);

		if (dist > sphere.GetRadius() * sphere.GetRadius() || dist < FLT_EPSILON)
			return false;

		newContact.location = closestPointWorld;
		newContact.normal = glm::normalize(sphere.GetPosition() - closestPointWorld);
		newContact.peneteration = sphere.GetRadius() - sqrt(dist);

		return true;
	}

	// Box vs Box contact generation

	void SortVertices(glm::vec3* verts0, int* vertIndexs0)
	{
		int faces[6][4] = {{4, 0, 3, 7}, {1, 5, 6, 2}, {0, 1, 2, 3}, {7, 6, 5, 4}, {5, 1, 0, 4}, {6, 7, 3, 2}};

		int       faceSet = -1;
		glm::vec3 temp[4]; // New correct clockwise order

		// Work out which face to use
		for (int i = 0; i < 6 && faceSet == -1; i++)
		{
			int numFound = 0;
			for (int j = 0; j < 4; j++)
			{
				if (vertIndexs0[j] == faces[i][j])
				{
					temp[numFound] = verts0[j];
					numFound++;

					if (numFound == 4)
					{
						faceSet = i;
						break;
					}
				}
			}
		}

		if (faceSet < 0)
		{
			int errorHasOccured = 1;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				verts0[i] = temp[i];
			}
		}
	}

	/***************************************************************************/

	inline bool VertInsideFace(glm::vec3* verts0, glm::vec3& p0, float planeErr = 0.0f)
	{
		// Work out the normal for the face
		glm::vec3 v0 = verts0[1] - verts0[0];
		glm::vec3 v1 = verts0[2] - verts0[0];
		glm::vec3 n = glm::cross(v1, v0);
		n = glm::normalize(n);

		for (int i = 0; i < 4; i++)
		{
			glm::vec3 s0 = verts0[i];
			glm::vec3 s1 = verts0[(i + 1) % 4];
			glm::vec3 sx = s0 + n * 10.0f;

			// Work out the normal for the face
			glm::vec3 sv0 = s1 - s0;
			glm::vec3 sv1 = sx - s0;
			glm::vec3 sn = glm::cross(sv1, sv0);
			sn = glm::normalize(sn);

			float d = glm::dot(s0, sn);
			float d0 = glm::dot(p0, sn) - d;

			// Outside the plane
			if (d0 > planeErr)
			{
				return false;
			}
		}

		return true;
	}

	inline void ClipFaceFaceVerts(glm::vec3* verts0, int* vertIndexs0, glm::vec3* verts1, int* vertIndexs1, glm::vec3* vertsX, int* numVertsX)
	{
		SortVertices(verts0, vertIndexs0);
		SortVertices(verts1, vertIndexs1);

		// Work out the normal for the face
		glm::vec3 v0 = verts0[1] - verts0[0];
		glm::vec3 v1 = verts0[2] - verts0[0];
		glm::vec3 n = glm::cross(v1, v0);
		n = glm::normalize(n);

		// Project all the vertices onto a shared plane, plane0
		glm::vec3 vertsTemp1[4];
		for (int i = 0; i < 4; i++)
		{
			vertsTemp1[i] = verts1[i] + (n * glm::dot(n, verts0[0] - verts1[i]));
		}

		static glm::vec3 temp[50];
		int              numVerts = 0;

		for (int c = 0; c < 2; c++)
		{
			glm::vec3* vertA = vertsTemp1;
			glm::vec3* vertB = verts0;
			if (c == 1)
			{
				vertA = verts0;
				vertB = vertsTemp1;
			}

			// Work out the normal for the face
			glm::vec3 v0 = vertA[1] - vertA[0];
			glm::vec3 v1 = vertA[2] - vertA[0];
			glm::vec3 n = glm::cross(v1, v0);
			n = glm::normalize(n);

			for (int i = 0; i < 4; i++)
			{
				glm::vec3 s0 = vertA[i];
				glm::vec3 s1 = vertA[(i + 1) % 4];
				glm::vec3 sx = s0 + n * 10.0f;

				// Work out the normal for the face
				glm::vec3 sv0 = s1 - s0;
				glm::vec3 sv1 = sx - s0;
				glm::vec3 sn = glm::cross(sv1, sv0);
				sn = glm::normalize(sn);

				float d = glm::dot(s0, sn);

				for (int j = 0; j < 4; j++)
				{
					glm::vec3 p0 = vertB[j];
					glm::vec3 p1 = vertB[(j + 1) % 4]; // Loops back to the 0th for the last one

					float d0 = glm::dot(p0, sn) - d;
					float d1 = glm::dot(p1, sn) - d;

					// Check there on opposite sides of the plane
					if ((d0 * d1) < 0.0f)
					{
						glm::vec3 pX = p0 + (glm::dot(sn, (s0 - p0)) / glm::dot(sn, (p1 - p0))) * (p1 - p0);

						if (VertInsideFace(vertA, pX, 0.1f))
						{
							temp[numVerts] = pX;
							numVerts++;
						}
					}

					if (VertInsideFace(vertA, p0))
					{
						temp[numVerts] = p0;
						numVerts++;
					}
				}
			}
		}

		// Remove verts which are very close to each other
		for (int i = 0; i < numVerts; i++)
		{
			for (int j = i; j < numVerts; j++)
			{
				if (i != j)
				{
					float dist = glm::length2((temp[i] - temp[j]));

					if (dist < 6.5f)
					{
						for (int k = j; k < numVerts; k++)
						{
							temp[k] = temp[k + 1];
						}
						numVerts--;
					}
				}
			}
		}

		*numVertsX = numVerts;

		for (int i = 0; i < numVerts; i++)
		{
			vertsX[i] = temp[i];
		}
	}

	inline void ClipFaceFaceVertsOld(glm::vec3* verts0, int* vertIndexs0, glm::vec3* verts1, int* vertIndexs1)
	{
		SortVertices(verts0, vertIndexs0);
		SortVertices(verts1, vertIndexs1);

		// Work out the normal for the face
		glm::vec3 v0 = verts1[1] - verts1[0];
		glm::vec3 v1 = verts1[2] - verts1[0];
		glm::vec3 n = glm::cross(v1, v0);
    n = glm::normalize(n);
		n = glm::normalize(n);

		glm::vec3 temp[4];

		for (int i = 0; i < 4; i++)
		{
			glm::vec3 s0 = verts1[i];
			glm::vec3 s1 = verts1[(i + 1) % 4];
			glm::vec3 sx = s0 + n * 10.0f;

			// Work out the normal for the face
			glm::vec3 sv0 = s1 - s0;
			glm::vec3 sv1 = sx - s0;
			glm::vec3 sn = glm::cross(sv1, sv0);
			sn = glm::normalize(sn);

			float d = glm::dot(s0, sn);

			for (int r = 0; r < 4; r++)
			{
				temp[r] = verts0[r];
			}

			for (int j = 0; j < 4; j++)
			{
				glm::vec3 p0 = verts0[j];
				glm::vec3 p1 = verts0[(j + 1) % 4]; // Loops back to the 0th for the last one

				float d0 = glm::dot(p0, sn) - d;
				float d1 = glm::dot(p1, sn) - d;

				// Check there on opposite sides of the plane
				if ((d0 * d1) < 0.0f)
				{
					glm::vec3 pX = p0 + (glm::dot(sn, (s0 - p0)) / glm::dot(sn, (p1 - p0))) * (p1 - p0);

					if (d0 > 0.0f)
					{
						// verts0[j] = pX;
						temp[j] = pX;
					}
					else
					{
						// verts0[(j+1)%4] = pX;
						temp[(j + 1) % 4] = pX;
					}
				}
			}

			for (int r = 0; r < 4; r++)
			{
				verts0[r] = temp[r];
			}
		}
	}

	inline void ClosestPtPointOBB(const glm::vec3& point, const BoxColliderComponent& box0, glm::vec3* closestP)
	{
		glm::vec3 c = box0.GetPosition();
		glm::mat3 u = box0.GetRotationMatrix();
		glm::vec3 e = box0.GetHalfSize();

		glm::vec3 d = point - c;

		glm::vec3 q = c;

		for (int i = 0; i < 3; i++)
		{
			float dist = glm::dot(d, u[i]);

			if (dist > e[i])
				dist = e[i];
			if (dist < -e[i])
				dist = -e[i];

			q += dist * u[i];
		}

		*closestP = q;
	}

	void ClipLinePlane(const glm::vec3*            verts0, // 2 verts
	                   const int*                  vertIndexs0,
	                   const BoxColliderComponent& box0,
	                   const glm::vec3*            verts1, // 4 verts
	                   int*                        vertIndexs1,
	                   const BoxColliderComponent& box1,
	                   glm::vec3*                  vertsX,
	                   int*                        numVertX)
	{
		ClosestPtPointOBB(verts0[0], box1, &vertsX[0]);
		ClosestPtPointOBB(verts0[1], box1, &vertsX[1]);
		*numVertX = 2;
	}

	void ClosestPointLineLine(const glm::vec3* verts0, // 2 verts
	                          const glm::vec3* verts1, // 2 verts
	                          glm::vec3*       vertsX,
	                          int*             numVertX)
	{
		glm::vec3 p1 = verts0[0];
		glm::vec3 q1 = verts0[1];
		glm::vec3 p2 = verts1[0];
		glm::vec3 q2 = verts1[1];

		glm::vec3 d1 = q1 - p1;
		glm::vec3 d2 = q2 - p2;
		glm::vec3 r = p1 - p2;
		float     a = glm::dot(d1, d1);
		float     e = glm::dot(d2, d2);
		float     f = glm::dot(d2, r);

		const float epsilon = 0.00001f;

		float     s, t;
		glm::vec3 c1, c2;

		if (a <= epsilon && e <= epsilon)
		{
			s = t = 0.0f;
			c1 = p1;
			c2 = p2;

			vertsX[0] = c1;
			*numVertX = 1;
			return;
		}

		if (a <= epsilon)
		{
			s = 0.0f;
			t = f / e;
			t = glm::clamp(t, 0.0f, 1.0f);
		}
		else
		{
			float c = glm::dot(d1, r);
			if (e <= epsilon)
			{
				t = 0.0f;
				s = glm::clamp(-c / a, 0.0f, 1.0f);
			}
			else
			{
				float b = glm::dot(d1, d2);
				float denom = a * e - b * b;

				if (denom != 0.0f)
				{
					s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
				}
				else
				{
					s = 0.0f;
				}

				t = (b * s + f) / e;

				if (t < 0.0f)
				{
					t = 0.0f;
					s = glm::clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f)
				{
					t = 1.0f;
					s = glm::clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}

		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;

		vertsX[0] = (c1 + c2) * 0.5f;
		*numVertX = 1;
	}

	int GetNumIsCollidingPoints(const BoxColliderComponent& box0, const glm::vec3& collisionNormal, const float penetration, glm::vec3 verts[8], int* vertIndexs)
	{
		glm::vec3 extents = box0.GetHalfSize();

		float x = extents.x;
		float y = extents.y;
		float z = extents.z;

		glm::vec3 Vertex[8] = {glm::vec3(-x, -y, -z),
		                       glm::vec3(x, -y, -z),
		                       glm::vec3(x, -y, z),
		                       glm::vec3(-x, -y, z),

		                       glm::vec3(-x, y, -z),
		                       glm::vec3(x, y, -z),
		                       glm::vec3(x, y, z),
		                       glm::vec3(-x, y, z)};

		for (int i = 0; i < 8; i++)
		{
			Vertex[i] = box0.GetWorldMatrix() * glm::vec4(Vertex[i], 1.0f);
		}

		glm::vec3 planePoint = Vertex[0];
		float     maxdist = glm::dot(Vertex[0], collisionNormal);

		for (int i = 0; i < 8; i++)
		{
			float d = glm::dot(Vertex[i], collisionNormal);
			if (d > maxdist)
			{
				maxdist = d;
				planePoint = Vertex[i];
			}
		}

		// Plane Equation (A dot N) - d = 0;

		float d = glm::dot(planePoint, collisionNormal);
		d -= penetration + 0.01f;

		int numVerts = 0;
		for (int i = 0; i < 8; i++)
		{
			float side = glm::dot(Vertex[i], collisionNormal) - d;

			if (side > 0)
			{
				verts[numVerts] = Vertex[i];
				vertIndexs[numVerts] = i;
				numVerts++;
			}
		}

		return numVerts;
	}

	void CalculateIsCollidingPoint(const BoxColliderComponent& box0,
	                       const BoxColliderComponent& box1,
	                       const float                 penetration,
	                       glm::vec3&                  collisionNormal,
	                       std::vector<glm::vec3>&     collisionPoints)
	{
		glm::vec3 verts0[8];
		int       vertIndex0[8];
		glm::vec3 norm0 = collisionNormal;
		int       numVerts0 = GetNumIsCollidingPoints(box0, norm0, penetration, verts0, vertIndex0);

		glm::vec3 verts1[8];
		int       vertIndex1[8];
		glm::vec3 norm1 = -collisionNormal;
		int       numVerts1 = GetNumIsCollidingPoints(box1, norm1, penetration, verts1, vertIndex1);

		// This should never really happen!
		if (numVerts0 == 0 || numVerts1 == 0)
		{
			return;
		}

		int        numVertsX = numVerts0;
		glm::vec3* vertsX = verts0;

		if (numVerts0 >= 4 && numVerts1 >= 4)
		{
			glm::vec3 clipVerts[50];
			ClipFaceFaceVerts(verts0, vertIndex0, verts1, vertIndex1, clipVerts, &numVertsX);
			vertsX = clipVerts;
		}

		{
			if (numVerts1 < numVerts0)
			{
				numVertsX = numVerts1;
				vertsX = verts1;
				collisionNormal = -norm1;
			}

			if (numVerts1 == 2 && numVerts0 == 2)
			{
				glm::vec3 V[2];
				int       numV = 0;

				ClosestPointLineLine(verts0, verts1, V, &numV);

				vertsX = V;
				numVertsX = numV;
			}

			if (numVerts0 == 2 && numVerts1 == 4)
			{
				ClipLinePlane(verts0, vertIndex0, box0, verts1, vertIndex1, box1, vertsX, &numVertsX);
			}

			if (numVerts0 == 4 && numVerts1 == 2)
			{
				ClipLinePlane(verts1, vertIndex1, box1, verts0, vertIndex0, box0, vertsX, &numVertsX);
			}

			for (int i = 0; i < numVertsX; i++)
			{
				collisionPoints.push_back(vertsX[i]);
			}
		}
	}

	inline void CalculateProjectionForBox(const BoxColliderComponent& box, const glm::vec3& axis, float& min, float& max)
	{
		glm::vec3 extents = box.GetHalfSize();

		float x = extents.x;
		float y = extents.y;
		float z = extents.z;

		glm::vec3 Vertex[8] = {glm::vec3(x, y, z),
		                       glm::vec3(-x, y, z),
		                       glm::vec3(x, -y, z),
		                       glm::vec3(-x, -y, z),

		                       glm::vec3(x, y, -z),
		                       glm::vec3(-x, y, -z),
		                       glm::vec3(x, -y, -z),
		                       glm::vec3(-x, -y, -z)};

		for (int i = 0; i < 8; i++)
		{
			Vertex[i] = box.GetWorldMatrix() * glm::vec4(Vertex[i], 1.0f);
		}

		min = max = glm::dot(Vertex[0], axis);
		for (int i = 0; i < 8; i++)
		{
			float d = glm::dot(Vertex[i], axis);

			if (d < min)
				min = d;
			if (d > max)
				max = d;
		}
	}

	inline bool ProjectOnAxis(const BoxColliderComponent& box0,
	                          const BoxColliderComponent& box1,
	                          const glm::vec3&            axisc,
	                          float*                      minPenetration = NULL,
	                          glm::vec3*                  axisPenetration = NULL,
	                          float*                      pen = NULL)
	{
		glm::vec3 axis = axisc;

		float lq = glm::length2(axis);
		if (lq <= 0.02f)
		{
			if (pen)
				*pen = 100000.0f;
			return true;
		}

		axis = glm::normalize(axis);

		float mina, maxa;
		float minb, maxb;
		CalculateProjectionForBox(box0, axis, mina, maxa);
		CalculateProjectionForBox(box1, axis, minb, maxb);

		float lena = maxa - mina;
		float lenb = maxb - minb;

		float minv = glm::min(mina, minb);
		float maxv = glm::max(maxa, maxb);
		float lenv = maxv - minv;

		if (lenv > (lena + lenb))
		{
			return false;
		}

		float penetration = (lena + lenb) - lenv;

		if (pen)
		{
			*pen = penetration;
		}

		if (minPenetration && axisPenetration)
		{
			if (penetration < *minPenetration)
			{
				*minPenetration = penetration;
				*axisPenetration = axis;

				// BoxA pushes BoxB away in the correct Direction
				if (minb < mina)
				{
					*axisPenetration *= -1;
				}
			}
		}

		return true;
	}

	bool BoxAndBox(const BoxColliderComponent& box1,
	               const BoxColliderComponent& box2,
	               std::vector<glm::vec3>&     collisionPoints,
	               float&                      penetration,
	               glm::vec3&                  collisionNormal)
	{
		glm::mat3 box1U = box1.GetRotationMatrix();
		glm::mat3 box2U = box2.GetRotationMatrix();

		bool      allAxesOverlapping = true;
		float     p = 10000.0f;
		glm::vec3 h;
		allAxesOverlapping &= ProjectOnAxis(box1, box2, box1U[0], &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, box1U[1], &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, box1U[2], &p, &h);

		allAxesOverlapping &= ProjectOnAxis(box1, box2, box2U[0], &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, box2U[1], &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, box2U[2], &p, &h);

		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[0], box2U[0]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[0], box2U[1]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[0], box2U[2]), &p, &h);

		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[1], box2U[0]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[1], box2U[1]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[1], box2U[2]), &p, &h);

		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[2], box2U[0]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[2], box2U[1]), &p, &h);
		allAxesOverlapping &= ProjectOnAxis(box1, box2, glm::cross(box1U[2], box2U[2]), &p, &h);

		if (allAxesOverlapping)
		{
			CalculateIsCollidingPoint(box1, box2, p, h, collisionPoints);
			penetration = p;
			collisionNormal = -h;
		}
		return allAxesOverlapping;
	}

	void AddOrUpdateConstraint(std::vector<ContactConstraint>& constraints, EkBody& bodyA, EkBody& bodyB, glm::vec3 point, glm::vec3 collisionNormal, float penetration)
	{
		auto iter =
		    std::find_if(constraints.begin(), constraints.end(), [&bodyA, &bodyB](const ContactConstraint& constraint) { return constraint.BodiesMatch(bodyA, bodyB); });

		if (iter == constraints.end())
		{
			ContactConstraint& constraint = constraints.emplace_back(bodyA, bodyB);
			constraint.AddCollisionPoint(point, collisionNormal, penetration);
		}
		else
		{
			iter->AddCollisionPoint(point, collisionNormal, penetration);
		}
	}
	std::vector<ContactConstraint> CreateConstraints(std::vector<EkBody*>& bodies)
	{
		std::vector<ContactConstraint> constraints;

		for (int first = 0; first < bodies.size(); ++first)
		{
			EkBody* firstBody = bodies[first];

			for (int second = first + 1; second < bodies.size(); ++second)
			{
				EkBody* secondBody = bodies[second];

				const BaseColliderComponent* firstCollider = firstBody->GetOwner().GetComponent<BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID);
				const BaseColliderComponent* secondCollider = secondBody->GetOwner().GetComponent<BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID);

				std::vector<glm::vec3> collisionPointsForPair;
				float                  maxPen = 0.0f;
				glm::vec3              collisionNormal;

				if (firstCollider->GetType() == EColliderType::BOX && secondCollider->GetType() == EColliderType::BOX)
				{
					const BoxColliderComponent* box1 = static_cast<const BoxColliderComponent*>(firstCollider);
					const BoxColliderComponent* box2 = static_cast<const BoxColliderComponent*>(secondCollider);

					bool IsColliding = BoxAndBox(*box1, *box2, collisionPointsForPair, maxPen, collisionNormal);

					if (IsColliding)
					{
						for (glm::vec3 collisionPoint : collisionPointsForPair)
						{
							AddOrUpdateConstraint(constraints, *firstBody, *secondBody, collisionPoint, collisionNormal, maxPen);
						}
					}
				}
				else if (firstCollider->GetType() == EColliderType::SPHERE && secondCollider->GetType() == EColliderType::SPHERE)
				{
					const SphereColliderComponent* sphere1 = static_cast<const SphereColliderComponent*>(firstCollider);
					const SphereColliderComponent* sphere2 = static_cast<const SphereColliderComponent*>(secondCollider);

					CollisionPointInfo info;

					bool IsColliding = SphereAndSphere(*sphere1, *sphere2, info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *firstBody, *secondBody, info.location, info.normal, info.peneteration);
				}

				else if (firstCollider->GetType() == EColliderType::BOX && secondCollider->GetType() == EColliderType::SPHERE)
				{
					const SphereColliderComponent* sphere = static_cast<const SphereColliderComponent*>(secondCollider);
					const BoxColliderComponent*    box = static_cast<const BoxColliderComponent*>(firstCollider);

					CollisionPointInfo info;

					bool IsColliding = SphereAndBox(*box, *sphere, info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *secondBody, *firstBody, info.location, info.normal, info.peneteration);
				}

				else if (firstCollider->GetType() == EColliderType::SPHERE && secondCollider->GetType() == EColliderType::BOX)
				{
					const SphereColliderComponent* sphere = static_cast<const SphereColliderComponent*>(firstCollider);
					const BoxColliderComponent*    box = static_cast<const BoxColliderComponent*>(secondCollider);

					CollisionPointInfo info;

					bool IsColliding = SphereAndBox(*box, *sphere, info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *firstBody, *secondBody, info.location, info.normal, info.peneteration);
				}
			};
		}

		return constraints;
	}

} // namespace Eklavya::Physics::ContactGenerator
