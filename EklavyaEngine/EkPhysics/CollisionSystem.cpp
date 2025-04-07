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

    bool BoxAndSphere(const OBB &box, const Sphere &sphere)
    {
        glm::vec3 localSpherePosition = box.GetPointInLocalSpace(sphere.center);

        bool xIsGreater = abs(localSpherePosition.x) - sphere.radius > box.halfExtents.x;
        bool yIsGreater = abs(localSpherePosition.y) - sphere.radius > box.halfExtents.y;
        bool zIsGreater = abs(localSpherePosition.z) - sphere.radius > box.halfExtents.z;

        return !xIsGreater && !yIsGreater && !zIsGreater;
    }

    bool SphereAndSphere(const Sphere &one, const Sphere &two)
    {
        glm::vec3 midline = one.center - two.center;
        float totalRadius = one.radius + two.radius;
        return glm::length2(midline) < totalRadius * totalRadius;
    }

    bool SphereAndSphere(const Sphere &sphereOne, const Sphere &sphereTwo,
                         CollisionPointInfo &newContact)
    {
        if (!SphereAndSphere(sphereOne, sphereTwo))
            return false;

        glm::vec3 p1 = sphereOne.center;
        glm::vec3 p2 = sphereTwo.center;

        glm::vec3 toSphereTwo = p2 - p1;
        float size = glm::length(toSphereTwo);

        newContact.normal = -1.0f * (toSphereTwo / size);
        newContact.location = p1 + (toSphereTwo / 2.0f);
        newContact.peneteration = (sphereOne.radius + sphereTwo.radius) - size;

        return true;
    }

    bool SphereAndBox(const OBB &box, const Sphere &sphere,
                      CollisionPointInfo &newContact)
    {
        // early test
        if (!BoxAndSphere(box, sphere))
            return false;

        glm::vec3 relativePos = box.GetPointInLocalSpace(sphere.center);
        glm::vec3 closestPoint = glm::clamp(relativePos, -1.0f * box.halfExtents, box.halfExtents);

        glm::vec3 closestPointWorld = box.GetPointInWorldSpace(closestPoint);

        float dist = glm::length2(closestPoint - relativePos);

        if (dist > sphere.radius * sphere.radius || dist < FLT_EPSILON)
            return false;

        newContact.location = closestPointWorld;
        newContact.normal = glm::normalize(sphere.center - closestPointWorld);
        newContact.peneteration = sphere.radius - sqrt(dist);

        return true;
    }

    // Helper functions for Box vs Box collision
    void SortVertices(glm::vec3 *verts0, int *vertIndexs0)
    {
        int faces[6][4] = {{4, 0, 3, 7}, {1, 5, 6, 2}, {0, 1, 2, 3}, {7, 6, 5, 4}, {5, 1, 0, 4}, {6, 7, 3, 2}};

        int faceSet = -1;
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

        if (faceSet >= 0)
        {
            for (int i = 0; i < 4; i++)
            {
                verts0[i] = temp[i];
            }
        }
    }

    bool VertInsideFace(glm::vec3 *verts0, glm::vec3 &p0, float planeErr = 0.0f)
    {
        glm::vec3 v0 = verts0[1] - verts0[0];
        glm::vec3 v1 = verts0[2] - verts0[0];
        glm::vec3 n = glm::cross(v1, v0);
        n = glm::normalize(n);

        for (int i = 0; i < 4; i++)
        {
            glm::vec3 s0 = verts0[i];
            glm::vec3 s1 = verts0[(i + 1) % 4];
            glm::vec3 sx = s0 + n * 10.0f;

            glm::vec3 sv0 = s1 - s0;
            glm::vec3 sv1 = sx - s0;
            glm::vec3 sn = glm::cross(sv1, sv0);
            sn = glm::normalize(sn);

            float d = glm::dot(s0, sn);
            float d0 = glm::dot(p0, sn) - d;

            if (d0 > planeErr)
            {
                return false;
            }
        }

        return true;
    }

    void ClipFaceFaceVerts(glm::vec3 *verts0, int *vertIndexs0, glm::vec3 *verts1, int *vertIndexs1,
                           glm::vec3 *vertsX, int *numVertsX)
    {
        SortVertices(verts0, vertIndexs0);
        SortVertices(verts1, vertIndexs1);

        glm::vec3 v0 = verts0[1] - verts0[0];
        glm::vec3 v1 = verts0[2] - verts0[0];
        glm::vec3 n = glm::cross(v1, v0);
        n = glm::normalize(n);

        glm::vec3 vertsTemp1[4];
        for (int i = 0; i < 4; i++)
        {
            vertsTemp1[i] = verts1[i] + (n * glm::dot(n, verts0[0] - verts1[i]));
        }

        static glm::vec3 temp[50];
        int numVerts = 0;

        for (int c = 0; c < 2; c++)
        {
            glm::vec3 *vertA = vertsTemp1;
            glm::vec3 *vertB = verts0;
            if (c == 1)
            {
                vertA = verts0;
                vertB = vertsTemp1;
            }

            glm::vec3 v0 = vertA[1] - vertA[0];
            glm::vec3 v1 = vertA[2] - vertA[0];
            glm::vec3 n = glm::cross(v1, v0);
            n = glm::normalize(n);

            for (int i = 0; i < 4; i++)
            {
                glm::vec3 s0 = vertA[i];
                glm::vec3 s1 = vertA[(i + 1) % 4];
                glm::vec3 sx = s0 + n * 10.0f;

                glm::vec3 sv0 = s1 - s0;
                glm::vec3 sv1 = sx - s0;
                glm::vec3 sn = glm::cross(sv1, sv0);
                sn = glm::normalize(sn);

                float d = glm::dot(s0, sn);

                for (int j = 0; j < 4; j++)
                {
                    glm::vec3 p0 = vertB[j];
                    glm::vec3 p1 = vertB[(j + 1) % 4];

                    float d0 = glm::dot(p0, sn) - d;
                    float d1 = glm::dot(p1, sn) - d;

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

        // Remove duplicate verts
        for (int i = 0; i < numVerts; i++)
        {
            for (int j = i; j < numVerts; j++)
            {
                if (i != j && glm::length2((temp[i] - temp[j])) < 6.5f)
                {
                    for (int k = j; k < numVerts; k++)
                    {
                        temp[k] = temp[k + 1];
                    }
                    numVerts--;
                }
            }
        }

        *numVertsX = numVerts;
        for (int i = 0; i < numVerts; i++)
        {
            vertsX[i] = temp[i];
        }
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

    int GetNumIsCollidingPoints(const OBB &box, const glm::vec3 &collisionNormal,
                                const float penetration, glm::vec3 verts[8], int *vertIndexs)
    {
        glm::vec3 extents = box.halfExtents;
        float x = extents.x;
        float y = extents.y;
        float z = extents.z;

        glm::vec3 Vertex[8] = {
            glm::vec3(-x, -y, -z),
            glm::vec3(x, -y, -z),
            glm::vec3(x, -y, z),
            glm::vec3(-x, -y, z),
            glm::vec3(-x, y, -z),
            glm::vec3(x, y, -z),
            glm::vec3(x, y, z),
            glm::vec3(-x, y, z)
        };

        for (int i = 0; i < 8; i++)
        {
            Vertex[i] = box.worldTransform * glm::vec4(Vertex[i], 1.0f);
        }

        glm::vec3 planePoint = Vertex[0];
        float maxdist = glm::dot(Vertex[0], collisionNormal);

        for (int i = 0; i < 8; i++)
        {
            float d = glm::dot(Vertex[i], collisionNormal);
            if (d > maxdist)
            {
                maxdist = d;
                planePoint = Vertex[i];
            }
        }

        float d = glm::dot(planePoint, collisionNormal) - penetration - 0.01f;

        int numVerts = 0;
        for (int i = 0; i < 8; i++)
        {
            if (glm::dot(Vertex[i], collisionNormal) - d > 0)
            {
                verts[numVerts] = Vertex[i];
                vertIndexs[numVerts] = i;
                numVerts++;
            }
        }

        return numVerts;
    }

    void CalculateIsCollidingPoint(const OBB &box0, const OBB &box1,
                                   const float penetration,
                                   glm::vec3 &collisionNormal,
                                   std::vector<glm::vec3> &collisionPoints)
    {
        glm::vec3 verts0[8];
        int vertIndex0[8];
        int numVerts0 = GetNumIsCollidingPoints(box0, collisionNormal, penetration, verts0, vertIndex0);

        glm::vec3 verts1[8];
        int vertIndex1[8];
        glm::vec3 norm1 = -collisionNormal;
        int numVerts1 = GetNumIsCollidingPoints(box1, norm1, penetration, verts1, vertIndex1);

        if (numVerts0 == 0 || numVerts1 == 0) return;

        int numVertsX = numVerts0;
        glm::vec3 *vertsX = verts0;

        if (numVerts0 >= 4 && numVerts1 >= 4)
        {
            glm::vec3 clipVerts[50];
            ClipFaceFaceVerts(verts0, vertIndex0, verts1, vertIndex1, clipVerts, &numVertsX);
            vertsX = clipVerts;
        }
        else
        {
            if (numVerts1 < numVerts0)
            {
                numVertsX = numVerts1;
                vertsX = verts1;
                collisionNormal = -norm1;
            }
        }

        for (int i = 0; i < numVertsX; i++)
        {
            collisionPoints.push_back(vertsX[i]);
        }
    }

    void CalculateProjectionForBox(const OBB &box, const glm::vec3 &axis, float &min, float &max)
    {
        glm::vec3 extents = box.halfExtents;
        float x = extents.x;
        float y = extents.y;
        float z = extents.z;

        glm::vec3 Vertex[8] = {
            glm::vec3(x, y, z),
            glm::vec3(-x, y, z),
            glm::vec3(x, -y, z),
            glm::vec3(-x, -y, z),
            glm::vec3(x, y, -z),
            glm::vec3(-x, y, -z),
            glm::vec3(x, -y, -z),
            glm::vec3(-x, -y, -z)
        };

        for (int i = 0; i < 8; i++)
        {
            Vertex[i] = box.worldTransform * glm::vec4(Vertex[i], 1.0f);
        }

        min = max = glm::dot(Vertex[0], axis);
        for (int i = 0; i < 8; i++)
        {
            float d = glm::dot(Vertex[i], axis);
            min = glm::min(min, d);
            max = glm::max(max, d);
        }
    }

    bool ProjectOnAxis(const OBB &box0, const OBB &box1, const glm::vec3 &axis,
                       float *minPenetration = NULL, glm::vec3 *axisPenetration = NULL)
    {
        if (glm::length2(axis) <= 0.02f) return true;

        glm::vec3 normalizedAxis = glm::normalize(axis);

        float mina, maxa, minb, maxb;
        CalculateProjectionForBox(box0, normalizedAxis, mina, maxa);
        CalculateProjectionForBox(box1, normalizedAxis, minb, maxb);

        float lena = maxa - mina;
        float lenb = maxb - minb;
        float lenv = glm::max(maxa, maxb) - glm::min(mina, minb);

        if (lenv > (lena + lenb)) return false;

        float penetration = (lena + lenb) - lenv;

        if (minPenetration && axisPenetration && penetration < *minPenetration)
        {
            *minPenetration = penetration;
            *axisPenetration = normalizedAxis * (minb < mina ? -1.0f : 1.0f);
        }

        return true;
    }

    bool BoxAndBox(const OBB &box1, const OBB &box2,
                   std::vector<glm::vec3> &collisionPoints,
                   float &penetration,
                   glm::vec3 &collisionNormal)
    {
        glm::mat3 box1U = box1.rotation;
        glm::mat3 box2U = box2.rotation;

        bool allAxesOverlapping = true;
        float p = 10000.0f;
        glm::vec3 h;

        // Test all 15 separating axes
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
} // namespace Eklavya::Physics::CollisionSystem
