//
//  ContactGenerator.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef ContactGenerator_h
#define ContactGenerator_h

#include <EkPhysics/Constraints/ContactConstraint.h>

namespace Eklavya::Physics
{
    class EkBody;

    namespace ContactGenerator
    {
        bool GetNearestContactToSphere(const std::vector<EkBody *> &bodies, glm::vec3 sphereCenter,
                                       float sphereRadius, glm::vec3 &nearestContact);

        std::vector<ContactConstraint> CreateConstraints(const std::vector<EkBody *> &bodies);
    } // namespace ContactGenerator
} // namespace Eklavya::Physics

#endif /* ContactGenerator_h */
