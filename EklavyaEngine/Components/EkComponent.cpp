//
//  EkComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 20/05/2023.
//

#include "EkComponent.hpp"

namespace Eklavya
{

	EkComponent::EkComponent(EkActor& owner, const Eklavya::EkComponentID id) : mOwner(owner), mComponentID(id)
	{
	}

	bool EkComponent::operator==(const EkComponent& rhs) const
	{
		return rhs.mComponentID == mComponentID;
	}

	bool EkComponent::operator==(const EkComponentID& otherCompID) const
	{
		return otherCompID == mComponentID;
	}

} // namespace Eklavya
