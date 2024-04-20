#ifndef INC_CONSTRAINT_H_
#define INC_CONSTRAINT_H_

#include "EkBody.h"
#include <glm/glm.hpp>

namespace Eklavya::Physics {

struct ContactData {
  glm::vec3 point;
  glm::vec3 normal;
  float depth;
  EkBody *bodyA;
  EkBody *bodyB;
};

class Constraint {
public:
  virtual void PreSolve() = 0;
  virtual void Solve() = 0;
  virtual void PostSolve() = 0;
};

class ContactConstraint : public Constraint {

public:
  ContactConstraint(ContactData &data);
  // Inherited via Constraint
  virtual void PreSolve() override;
  virtual void Solve() override;
  virtual void PostSolve() override;

private:
  ContactData &m_manifold;
};
} // namespace Eklavya::Physics

#endif
