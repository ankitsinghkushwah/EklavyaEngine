#include "IAsset.h"
#include <cstdio>

using namespace Eklavya::Asset;

IAsset::IAsset(EType type, const std::string &name) :
	m_Type(type), m_Name(name) {}

IAsset::~IAsset() {}
