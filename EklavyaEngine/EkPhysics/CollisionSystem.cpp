//
//  CollisionSystem.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#include "CollisionSystem.h"
#include "Collider.h"
#include <EkPhysics/EkBody.h>
#include <Scene/EkActor.h>

#include "glm/gtc/matrix_access.hpp"
#include <algorithm>
#include <cfloat>

#ifdef EKDEBUG
#include <Renderer/DebugRenderer.hpp>
#endif

namespace Eklavya::Physics::CollisionSystem
{
	bool RayVsOBB(Ray ray, const BoxColliderComponent &boxCollider, float &t)
	{
		ray.d = glm::normalize(ray.d);

		float tMin = -FLT_MAX;
		float tMax = FLT_MAX;

		const glm::mat4 &worldMatrix = boxCollider.GetOwner().Transform().GetWorldMatrix();

		glm::vec3 center = worldMatrix[3];

		for (int i = 0; i < 3; i++)
		{
			float t1 = -FLT_MAX;
			float t2 = FLT_MAX;

			glm::vec3 normal = glm::column(worldMatrix, i);
			glm::vec3 delta = center - ray.o;
			float l = glm::length(normal) * 0.5f;
			normal = glm::normalize(normal);

			const float min = -l;
			const float max = l;

			float e = glm::dot(normal, delta);
			float f = glm::dot(ray.d, normal);
			if (glm::abs(f) > std::numeric_limits<float>::epsilon())
			{
				float t1 = (e + min) / f;
				float t2 = (e + max) / f;

				if (t1 > t2)
				{
					std::swap(t1, t2);
				}

				if (t1 > tMin)
				{
					tMin = t1;
				}

				if (t2 < tMax)
				{
					tMax = t2;
				}

				if (tMax < tMin)
				{
					return false;
				}
			}
			else
			{
				if (-e + min > 0.0f || -e + max < 0.0f)
					return false;
			}
		}

		t = tMin;
		if (t > 0.0001f)
			return true;
		return false;
	}

	bool RayVsSphere(Ray ray, glm::vec3 sphereCenter, float radius, glm::vec2 &points)
	{
		glm::vec3 L = ray.o - sphereCenter;

		float a = glm::dot(ray.d, ray.d);
		float b = 2.0f * glm::dot(ray.d, L);
		float c = glm::dot(L, L) - radius * radius;

		float discr = b * b - 4 * a * c;
		if (discr < 0)
			return false;

		else if (discr == 0)
			points[0] = points[1] = -0.5 * b / a;
		else
		{
			points[0] = -0.5 * (b + sqrt(discr));
			points[1] = -0.5 * (b - sqrt(discr));
		}

		if (points[0] > points[1])
			std::swap(points[0], points[1]);

		return true;
	}

	static bool BoxOverlapsSphere(const OBB &box, const Sphere &sphere)
	{
		glm::vec3 localSpherePosition = box.GetPointInLocalSpace(sphere.center);

		bool xIsGreater = abs(localSpherePosition.x) - sphere.radius > box.halfExtents.x;
		bool yIsGreater = abs(localSpherePosition.y) - sphere.radius > box.halfExtents.y;
		bool zIsGreater = abs(localSpherePosition.z) - sphere.radius > box.halfExtents.z;

		return !xIsGreater && !yIsGreater && !zIsGreater;
	}

	static bool SpheresOverlap(const Sphere &one, const Sphere &two)
	{
		glm::vec3 midline = one.center - two.center;
		float totalRadius = one.radius + two.radius;
		return glm::length2(midline) < totalRadius * totalRadius;
	}

	bool SphereVsSphere(const Sphere &sphereOne, const Sphere &sphereTwo, ContactManifold &manifold)
	{
		if (!SpheresOverlap(sphereOne, sphereTwo))
			return false;

		glm::vec3 p1 = sphereOne.center;
		glm::vec3 p2 = sphereTwo.center;

		glm::vec3 toSphereTwo = p2 - p1;
		float size = glm::length(toSphereTwo);

		ContactPoint contact;
		contact.position = p1 + (toSphereTwo / 2.0f);
		contact.depth = (sphereOne.radius + sphereTwo.radius) - size;

		manifold.normal = toSphereTwo / size; // from sphereOne to sphereTwo
		manifold.points = {contact};

		return true;
	}

	bool BoxVsSphere(const OBB &box, const Sphere &sphere, ContactManifold &manifold)
	{
		// early test
		if (!BoxOverlapsSphere(box, sphere))
			return false;

		glm::vec3 relativePos = box.GetPointInLocalSpace(sphere.center);
		glm::vec3 closestPoint = glm::clamp(relativePos, -1.0f * box.halfExtents, box.halfExtents);

		glm::vec3 closestPointWorld = box.GetPointInWorldSpace(closestPoint);

		float dist = glm::length2(closestPoint - relativePos);

		if (dist > sphere.radius * sphere.radius || dist < FLT_EPSILON)
			return false;

		ContactPoint contact;
		contact.position = closestPointWorld;
		contact.depth = sphere.radius - sqrt(dist);

		manifold.normal = glm::normalize(sphere.center - closestPointWorld); // from the box to the sphere
		manifold.points = {contact};

		return true;
	}

	void ClosestPtPointOBB(const glm::vec3 &point, const OBB &box, glm::vec3 *closestP)
	{
		glm::vec3 c = box.center;
		glm::mat3 u = box.rotation;
		glm::vec3 e = box.halfExtents;

		glm::vec3 d = point - c;
		glm::vec3 q = c;

		for (int i = 0; i < 3; i++)
		{
			float dist = glm::dot(d, u[i]);
			dist = glm::clamp(dist, -e[i], e[i]);
			q += dist * u[i];
		}

		*closestP = q;
	}
} // namespace Eklavya::Physics::CollisionSystem

// ---- Box vs box contact generation (SAT + Sutherland-Hodgman clipping) ----

namespace Eklavya::Physics
{
	std::vector<glm::vec3> ClipPolygonAgainstPlane(const std::vector<glm::vec3> &polygon, const ClipPlane &plane)
	{
		std::vector<glm::vec3> output;

		for (int idx = 0; idx < polygon.size(); ++idx)
		{
			glm::vec3 v_from = polygon[idx];
			const int next_idx = ((idx + 1) % polygon.size());
			glm::vec3 v_to = polygon[next_idx];

			const float v_from_d = dot((v_from - plane.point), plane.normal);
			const float v_to_d = dot((v_to - plane.point), plane.normal);
			const bool is_v_from_in = v_from_d <= 0.0f;
			const bool is_v_to_in = v_to_d <= 0.0f;

			const bool in_to_out = is_v_from_in && is_v_to_in == false;
			const bool in_to_in = is_v_from_in && is_v_to_in;
			const bool out_to_in = is_v_from_in == false && is_v_to_in;
			const bool out_to_out = is_v_from_in == false && is_v_to_in == false;

			const float t = v_from_d / (v_from_d - v_to_d);
			const glm::vec3 intersected_point = v_from + t * (v_to - v_from);

			if (in_to_out)
			{
				output.push_back(intersected_point);
			}
			else if (in_to_in)
			{
				output.push_back(v_to);
			}
			else if (out_to_in)
			{
				output.push_back(intersected_point);
				output.push_back(v_to);
			}
		}

		return output;
	}

	SatResult BoxBoxSAT(const OBB &a, const OBB &b)
	{
		SatResult result;

		auto ProjectBoxOnAxis = [&](float &min, float &max, const OBB &box, const glm::vec3 &axis)
		{
			float x = box.halfExtents.x;
			float y = box.halfExtents.y;
			float z = box.halfExtents.z;

			glm::vec3 vertices[8] =
			{
				{x, y, z},
				{-x, y, z},
				{-x, -y, z},
				{x, -y, z},
				{x, y, -z},
				{-x, y, -z},
				{-x, -y, -z},
				{x, -y, -z}
			};

			for (int i = 0; i < 8; ++i)
			{
				glm::vec3 wp = box.worldTransform * glm::vec4(vertices[i], 1.0f);
				min = glm::min(min, dot(axis, wp));
				max = glm::max(max, dot(axis, wp));
			}
		};

		float smallest_pen = FLT_MAX;


		// An edge pair has to beat the best face by this factor to win.
		const float kEdgeMustBeatFaceBy = 0.99f;

		auto SAT = [&](const glm::vec3 &axis, ESatAxis type, int indexA, int indexB) -> bool
		{
			float minA = FLT_MAX, maxA = -FLT_MAX;
			float minB = FLT_MAX, maxB = -FLT_MAX;

			ProjectBoxOnAxis(minA, maxA, a, axis);
			ProjectBoxOnAxis(minB, maxB, b, axis);

			float c_distance = dot(b.center - a.center, axis);
			float pen = ((maxA - minA) * 0.5f + (maxB - minB) * 0.5f) - glm::abs(c_distance);

			if (pen <= 0.0f)
				return false;

			const float bias = (result.type == ESatAxis::FaceA || result.type == ESatAxis::FaceB) && type ==
			                   ESatAxis::EdgeEdge
				                   ? kEdgeMustBeatFaceBy
				                   : 1.0f;
			if (pen < smallest_pen * bias)
			{
				smallest_pen = pen;
				// The sign of c_distance says which side B is on: flip so the normal points from A to B.
				result.normal = c_distance < 0.0f ? -axis : axis;
				result.type = type;
				result.indexA = indexA;
				result.indexB = indexB;
			}

			return true;
		};

		// One separating axis is enough to prove the boxes are apart, so return at the first one.
		// Face axes go first, so the edge pairs are compared against the best face.

		// 3 face normals of A
		for (int i = 0; i < 3; ++i)
		{
			if (SAT(a.rotation[i], ESatAxis::FaceA, i, -1) == false)
				return result;
		}

		// 3 face normals of B
		for (int j = 0; j < 3; ++j)
		{
			if (SAT(b.rotation[j], ESatAxis::FaceB, -1, j) == false)
				return result;
		}

		// 9 edge pairs: the axis perpendicular to edge i of A and edge j of B
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				glm::vec3 axis = glm::cross(a.rotation[i], b.rotation[j]);

				// Parallel edges give a (near) zero cross product: no usable direction, and
				// normalising it would blow up. The face axes already cover this case.
				const float lengthSq = glm::dot(axis, axis);
				if (lengthSq < 1e-6f)
					continue;

				axis /= glm::sqrt(lengthSq);

				if (SAT(axis, ESatAxis::EdgeEdge, i, j) == false)
					return result;
			}
		}

		result.overlapping = true;
		result.penetration = smallest_pen;

		return result;
	}

	BoxFace GetBoxFace(const OBB &box, int axis, float sign)
	{
		BoxFace face;

		face.normal = box.rotation[axis] * sign;
		face.center = box.center + face.normal * box.halfExtents[axis];
		face.axis = axis;
		face.sign = sign;


		int u = (axis + 1) % 3;
		int v = (axis + 2) % 3;

		glm::vec3 du = box.rotation[u] * box.halfExtents[u];
		glm::vec3 dv = box.rotation[v] * box.halfExtents[v];


		if (sign >= 0.0f)
		{
			face.vertices[0] = face.center + du + dv;
			face.vertices[1] = face.center - du + dv;
			face.vertices[2] = face.center - du - dv;
			face.vertices[3] = face.center + du - dv;
		}

		if (sign < 0.0f)
		{
			face.vertices[0] = face.center + du + dv;
			face.vertices[1] = face.center + du - dv;
			face.vertices[2] = face.center - du - dv;
			face.vertices[3] = face.center - du + dv;
		}


		return face;
	}

	FacePair SelectFaces(const OBB &a, const OBB &b, const SatResult &sat)
	{
		FacePair pair;


		const OBB &ref_box = sat.type == ESatAxis::FaceA ? a : b;
		const OBB &inc_box = sat.type == ESatAxis::FaceA ? b : a;
		const int ref_axis_idx = sat.type == ESatAxis::FaceA ? sat.indexA : sat.indexB;
		glm::vec3 normal = sat.type == ESatAxis::FaceA ? sat.normal : -sat.normal;
		pair.flipped = sat.type == ESatAxis::FaceB;


		//reference plane
		float sign = glm::dot(ref_box.rotation[ref_axis_idx], normal) > 0.0f ? 1.0f : -1.0f;
		pair.reference = GetBoxFace(ref_box, ref_axis_idx, sign);

		glm::vec3 refNormal = ref_box.rotation[ref_axis_idx] * sign;

		//incident plane
		float final_dj = -FLT_MAX;
		float abs_dj = 0.0f;
		int index = 0;

		for (int i = 0; i < 3; ++i)
		{
			float d_j = glm::dot(inc_box.rotation[i], refNormal);
			if (glm::abs(d_j) > abs_dj)
			{
				abs_dj = glm::abs(d_j);
				final_dj = d_j;
				index = i;
			}
		}

		sign = final_dj > 0.0f ? -1.0f : 1.0f;
		pair.incident = GetBoxFace(inc_box, index, sign);

		return pair;
	}


	std::vector<glm::vec3> ClipIncidentFace(const FacePair &faces)
	{
		std::vector<glm::vec3> polygon(faces.incident.vertices.begin(), faces.incident.vertices.end());

		for (int v_idx = 0; v_idx < faces.reference.vertices.size(); ++v_idx)
		{
			glm::vec3 v0 = faces.reference.vertices[v_idx];
			glm::vec3 v1 = faces.reference.vertices[(v_idx + 1) % faces.reference.vertices.size()];


			ClipPlane plane;
			glm::vec3 to_v1 = glm::normalize(v1 - v0);
			plane.normal = glm::cross(to_v1, faces.reference.normal);
			plane.point = (v1 + v0) * 0.5f;

			polygon = ClipPolygonAgainstPlane(polygon, plane);
		}

		return polygon;
	}

	ContactManifold BuildFaceManifold(const FacePair &faces, const SatResult &sat,
	                                  const std::vector<glm::vec3> &clipped)
	{
		ContactManifold manifold;

		// sat.normal already points from A to B, whichever box owns the reference face.
		manifold.normal = sat.normal;

		const BoxFace &ref = faces.reference;

		for (const glm::vec3 &p: clipped)
		{
			// Signed distance of p from the reference face plane.
			const float separation = glm::dot(p - ref.center, ref.normal);

			// Drop the point if it is not below the reference face.
			if (separation > 0.0f)
			{
				continue;
			}

			ContactPoint contact;

			contact.position = p;
			contact.depth = -separation;

			manifold.points.push_back(contact);
		}

		return manifold;
	}

	BoxEdge GetSupportingEdge(const OBB &box, int axis, const glm::vec3 &towards)
	{
		BoxEdge edge;

		edge.direction = box.rotation[axis];
		edge.halfLength = box.halfExtents[axis];

		edge.midpoint = box.center;

		int second_axis = (axis + 1) % 3;
		int third_axis = (axis + 2) % 3;
		float second_axis_sign = glm::dot(box.rotation[second_axis], towards) > 0.0f ? 1.0f : -1.0f;
		float third_axis_sign = glm::dot(box.rotation[third_axis], towards) > 0.0f ? 1.0f : -1.0f;

		edge.midpoint = box.center + (second_axis_sign * box.halfExtents[second_axis] * box.rotation[second_axis]) + (
			                third_axis_sign * box.halfExtents[third_axis] * box.rotation[third_axis]);

		return edge;
	}

	void ClosestPointsBetweenEdges(const BoxEdge &e0, const BoxEdge &e1, glm::vec3 &pointOnE0, glm::vec3 &pointOnE1)
	{
		glm::vec3 r = e0.midpoint - e1.midpoint;
		float b = glm::dot(e0.direction, e1.direction);
		float c = glm::dot(r, e0.direction);
		float f = glm::dot(r, e1.direction);

		float denom = 1.0f - b * b;

		float s = 0.0f;

		if (denom > 0.0f)
		{
			s = (b * f - c) / denom;
		}
		float t = f + b * s;

		//clamping
		if (s < -e0.halfLength || s > e0.halfLength)
		{
			s = std::clamp(s, -e0.halfLength, e0.halfLength);
			t = f + b * s;
		}

		if (t < -e1.halfLength || t > e1.halfLength)
		{
			t = std::clamp(t, -e1.halfLength, e1.halfLength);
			s = b * t - c;
			s = std::clamp(s, -e0.halfLength, e0.halfLength);
		}

		pointOnE0 = e0.midpoint + s * e0.direction;
		pointOnE1 = e1.midpoint + t * e1.direction;
	}

	ContactManifold BuildEdgeManifold(const OBB &a, const OBB &b, const SatResult &sat)
	{
		ContactManifold manifold;
		manifold.normal = sat.normal;

		BoxEdge a_edge = GetSupportingEdge(a, sat.indexA, sat.normal);
		BoxEdge b_edge = GetSupportingEdge(b, sat.indexB, -sat.normal);

		glm::vec3 closest_point_on_a, closest_point_on_b;
		ClosestPointsBetweenEdges(a_edge, b_edge, closest_point_on_a, closest_point_on_b);

		ContactPoint edge_contact;
		edge_contact.position = (closest_point_on_a + closest_point_on_b) * 0.5f;
		edge_contact.depth = glm::dot((closest_point_on_a - closest_point_on_b), sat.normal);
		manifold.points.push_back(edge_contact);

		return manifold;
	}

	ContactManifold ReduceManifold(const ContactManifold &manifold)
	{
		const std::vector<ContactPoint> &points = manifold.points;
		const int count = static_cast<int>(points.size());

		if (count <= 4)
		{
			return manifold;
		}

		// #1: deepest point.
		int first = 0;
		for (int i = 1; i < count; ++i)
		{
			if (points[i].depth > points[first].depth)
			{
				first = i;
			}
		}

		// #2: farthest from #1 (squared distance is enough to compare).
		int second = -1;
		float best_distance = -1.0f;
		for (int i = 0; i < count; ++i)
		{
			if (i == first)
			{
				continue;
			}

			const glm::vec3 d = points[i].position - points[first].position;
			const float distance = glm::dot(d, d);
			if (distance > best_distance)
			{
				best_distance = distance;
				second = i;
			}
		}

		// Signed area of the triangle (#1, #2, p), measured along the contact normal.
		// Positive on one side of the #1-#2 line, negative on the other.
		const glm::vec3 edge = points[second].position - points[first].position;
		auto signed_area = [&](int i)
		{
			return glm::dot(glm::cross(edge, points[i].position - points[first].position), manifold.normal);
		};

		// #3: largest positive signed area.
		int third = -1;
		float best_area = -FLT_MAX;
		for (int i = 0; i < count; ++i)
		{
			if (i == first || i == second)
			{
				continue;
			}

			const float area = signed_area(i);
			if (area > best_area)
			{
				best_area = area;
				third = i;
			}
		}

		// #4: most negative signed area, i.e. farthest on the other side of the #1-#2 line.
		int fourth = -1;
		float worst_area = FLT_MAX;
		for (int i = 0; i < count; ++i)
		{
			if (i == first || i == second || i == third)
			{
				continue;
			}

			const float area = signed_area(i);
			if (area < worst_area)
			{
				worst_area = area;
				fourth = i;
			}
		}

		ContactManifold reduced;
		reduced.normal = manifold.normal;
		reduced.points = {points[first], points[third], points[second], points[fourth]};

		return reduced;
	}
} // namespace Eklavya::Physics

namespace Eklavya::Physics::CollisionSystem
{
	bool BoxVsBox(const OBB &box1, const OBB &box2, ContactManifold &manifold)
	{
		const SatResult sat = BoxBoxSAT(box1, box2);
		if (sat.overlapping == false)
			return false;

		if (sat.type == ESatAxis::EdgeEdge)
		{
			manifold = BuildEdgeManifold(box1, box2, sat);
		}
		else
		{
			const FacePair faces = SelectFaces(box1, box2, sat);
			const std::vector<glm::vec3> clipped = ClipIncidentFace(faces);
			manifold = BuildFaceManifold(faces, sat, clipped);
		}

		manifold = ReduceManifold(manifold);
		return true;
	}
} // namespace Eklavya::Physics::CollisionSystem
