#ifndef INC_RANDOM_H
#define INC_RANDOM_H

// C++ headers
#include <chrono>
#include <memory>
#include <random>

// third pary headers
#include <glm/glm.hpp>

class Random
{
  public:
	~Random();

	static std::shared_ptr<Random> GetInstance()
	{
		static std::shared_ptr<Random> instance(new Random);
		return instance;
	}

	void      Update();
	float     Real(float pMin, float pMax);
	int       Integer(int pMin, int pMax);
	glm::vec4 GetOnXZ(float min, float max);
	glm::vec3 GetPointOnUnitSphere();

  private:
	std::default_random_engine mRandEngine;
	Random();
};

#endif