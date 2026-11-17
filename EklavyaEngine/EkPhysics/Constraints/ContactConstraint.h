//
//  ContactConstraint.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef ContactConstraint_h
#define ContactConstraint_h

#include "Constraint.h"
#include <EkPhysics/ContactManifold.h>
#include <glm/glm.hpp>
#include <vector>

namespace Eklavya::Physics {

class EkBody;

class ContactConstraint : public Constraint {
public:
  // `manifold.normal` must point from bodyA to bodyB.
  ContactConstraint(EkBody &bodyA, EkBody &bodyB, const ContactManifold &manifold);

  const ContactManifold &GetManifold() const { return mManifold; }
  void PreSolve() override {}
  void Solve(float dt) override;
  void PostSolve() override {}

private:
  EkBody &mBodyA;
  EkBody &mBodyB;

  ContactManifold mManifold;
};
} // namespace Eklavya::Physics
#endif /* ContactConstraint_hpp */
