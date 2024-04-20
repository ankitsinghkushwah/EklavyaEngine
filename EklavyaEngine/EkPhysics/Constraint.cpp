#include "Constraint.h"

namespace Eklavya::Physics
{
  ContactConstraint::ContactConstraint(ContactData &manifold)
      : m_manifold(manifold)
  {
  }

  void ContactConstraint::PreSolve() {}

  /*
   BIG ASS IMPULSE FORMULA

   (1 + e) Vrel . n
   ____________________________________________________________
   Jn =        1/Ma + 1/Mb + |( Ra X n ).(Ra x n)| + |( Rb X n ).(Rb x n)|
   |___________________|   |___________________|
   |       Ia          |   |       Ib          |

   */

#define DYNAMIC_FRICTION
#define IMPOSE_TANGENT_FRICTION

  void ContactConstraint::Solve()
  {
    float SZ = 4.0f;

    auto &A = m_manifold.bodyA;
    auto &B = m_manifold.bodyB;
    
    if(A->ShouldPutOnSleepOnCollision())
    {
      A->SetAwake(false);
    }
    
    if(B->ShouldPutOnSleepOnCollision())
    {
      B->SetAwake(false);
    }
    

    float invMassA = A->GetInverseMass();
    float invMassB = B->GetInverseMass();
    float invMassSum = invMassA + invMassB;

    // both bodies are not moveable?
    if (invMassSum == 0.0f)
      {
        return;
      }

    glm::vec3 Ra = m_manifold.point - A->GetPosition();
    glm::vec3 Rb = m_manifold.point - B->GetPosition();
    glm::vec3 Va = A->GetVelocity() + glm::cross(A->GetAngularVel(), Ra);
    glm::vec3 Vb = B->GetVelocity() + glm::cross(B->GetAngularVel(), Rb);
    float     Ia = A->I();
    float     Ib = B->I();

    glm::vec3 n = -glm::normalize(m_manifold.normal);

    glm::vec3 RelV = Vb - Va;

    // are objects moving away from each other?
    if (glm::dot(RelV, n) > 0.0f)
      {
        return;
      }

    float e = A->GetRestitution() + B->GetRestitution();

    float d1 = glm::dot(glm::cross(Ra, n), glm::cross(Ra, n)) * Ia;
    float d2 = glm::dot(glm::cross(Rb, n), glm::cross(Rb, n)) * Ib;

    float J = (-(1.0f + e) * glm::dot(RelV, n));

    J /= (invMassSum + d1 + d2);

    J /= SZ;

    glm::vec3 Jn = J * n;

    glm::vec3 totalJ = Jn;

    // tangent
    glm::vec3 t = RelV - (n * glm::dot(RelV, n));

    if (glm::length(t) <= std::numeric_limits<float>::epsilon())
      {
        if (A->ShouldPutOnSleepOnCollision() == false)
          A->ApplyImpulse(-totalJ, Ra);
        if (B->ShouldPutOnSleepOnCollision() == false)
          B->ApplyImpulse(totalJ, Rb);
        return;
      }

    t = glm::normalize(t);

    d1 = glm::dot(glm::cross(Ra, t), glm::cross(Ra, t)) * Ia;
    d2 = glm::dot(glm::cross(Rb, t), glm::cross(Rb, t)) * Ib;

    float Jt = (-(1.0f + e) * glm::dot(RelV, t));

    Jt /= (invMassSum + d1 + d2);

    Jt /= SZ;

    totalJ += (Jt * t);

    if (A->ShouldPutOnSleepOnCollision() == false)
      A->ApplyImpulse(-totalJ, Ra);
    if (B->ShouldPutOnSleepOnCollision() == false)
      B->ApplyImpulse(totalJ, Rb);
  }

  void ContactConstraint::PostSolve()
  {
    auto     &A = m_manifold.bodyA;
    auto     &B = m_manifold.bodyB;

    auto      cNormal = -m_manifold.normal;
    auto      cPen = m_manifold.depth;

    float     totalMass = A->GetInverseMass() + B->GetInverseMass();

    float     scalar = (totalMass == 0.0f) ? 0.0f : cPen / totalMass;
    glm::vec3 correction = cNormal * scalar;

    glm::vec3 newAPos = A->GetPosition() - correction * A->GetInverseMass();
    glm::vec3 newBPos = B->GetPosition() + correction * B->GetInverseMass();

    A->SetPos(newAPos);
    B->SetPos(newBPos);
  }

} // namespace Eklavya::Physics
