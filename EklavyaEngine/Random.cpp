#include "Random.h"

using namespace std;

Random::Random()
{
	mRandEngine.seed(chrono::steady_clock::now().time_since_epoch().count());
}

Random::~Random()
{
}

void Random::Update()
{
	mRandEngine.seed(chrono::steady_clock::now().time_since_epoch().count());
}

float Random::Real(float pMin, float pMax)
{
	std::uniform_real_distribution<> distReal(pMin, pMax);
	return distReal(mRandEngine);
}

int Random::Integer(int pMin, int pMax)
{
	std::uniform_int_distribution<> distReal(pMin, pMax);
	return distReal(mRandEngine);
}

glm::vec4 Random::GetOnXZ(float min, float max)
{
	glm::vec4 result = glm::vec4();
	result.x = Real(min, max);
	result.z = Real(min, max);
	return result;
}

glm::vec3 Random::GetPointOnUnitSphere()
{
	glm::vec3 result = glm::vec3(0.0f);
	result.x = Real(-2.0f, 2.0f);
	result.z = Real(-2.0f, 2.0f);
	result.y = Real(-2.0f, 2.0f);
	return glm::normalize(result);
}