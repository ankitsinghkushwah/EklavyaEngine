#ifndef INC_HELPERS_H
#define INC_HELPERS_H

#include <memory>
#include <functional>
#include "Event/Events.h"
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

const int ACTOR_NOT_NEEDED = -1;
const int ACTOR_BOUNDING_VOLUME = -2;

#define MAX_FLOAT std::numeric_limits<float>::max();

template<typename Type>
inline std::shared_ptr<Type> MakeSharedPtr(std::weak_ptr<Type> weakObject)
{
	if (!weakObject.expired())
		return std::shared_ptr<Type>(weakObject);
	else
		return std::shared_ptr<Type>();
}

inline glm::vec3 GetGLMVec(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

inline glm::mat4 GetGLMMat(const aiMatrix4x4& from)
{
	glm::mat4 to;
	// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1;
	to[1][0] = from.a2;
	to[2][0] = from.a3;
	to[3][0] = from.a4;
	to[0][1] = from.b1;
	to[1][1] = from.b2;
	to[2][1] = from.b3;
	to[3][1] = from.b4;
	to[0][2] = from.c1;
	to[1][2] = from.c2;
	to[2][2] = from.c3;
	to[3][2] = from.c4;
	to[0][3] = from.d1;
	to[1][3] = from.d2;
	to[2][3] = from.d3;
	to[3][3] = from.d4;
	return to;
}

#endif
