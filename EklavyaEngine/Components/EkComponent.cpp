//
//  EkComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 20/05/2023.
//

#include "EkComponent.hpp"

namespace Eklavya {

EkComponent::EkComponent(const Eklavya::EkComponentID id) : m_ComponentID(id) {}

bool EkComponent::operator==(const EkComponent &rhs) const {
  return rhs.m_ComponentID == m_ComponentID;
}

bool EkComponent::operator==(const EkComponentID &otherCompID) const {
  return otherCompID == m_ComponentID;
}

} // namespace Eklavya
