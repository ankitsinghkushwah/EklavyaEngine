//
//  Asset.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 29/05/2023.
//
#ifndef Asset_h
#define Asset_h
#include <memory>
namespace Eklavya::Asset {
class Texture2D;
class ShaderProgram;
class Cubemap;
class GLModel;
class Animation;
} // namespace Eklavya::Asset
// assets
using SHARED_TEXTURE = std::shared_ptr<Eklavya::Asset::Texture2D>;
using SHARED_SHADER = std::shared_ptr<Eklavya::Asset::ShaderProgram>;
using SHARED_CUBEMAP = std::shared_ptr<Eklavya::Asset::Cubemap>;
using SHARED_MODEL = std::shared_ptr<Eklavya::Asset::GLModel>;
using SHARED_ANIMATION = std::shared_ptr<Eklavya::Asset::Animation>;
#endif /* Asset_h */
