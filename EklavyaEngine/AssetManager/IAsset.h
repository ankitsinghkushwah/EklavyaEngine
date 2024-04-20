#ifndef _INCLUDE_IASSET_H_
#define _INCLUDE_IASSET_H_

#include <string>

namespace Eklavya::Asset {
enum class EType {
  ASSET_TYPE_MIN = 0,
  TEXTURE = ASSET_TYPE_MIN,
  SHADER,
  CUBEMAP,
  MODEL,
  ANIMATION,
  ASSET_TYPE_MAX
};

class IAsset {
public:
  IAsset() = default;
  IAsset(EType type, const std::string &name);
  virtual ~IAsset();

  inline EType GetType() const { return m_Type; }

  inline std::string GetName() const { return m_Name; }

protected:
  EType m_Type;
  std::string m_Name;
};

} // namespace Eklavya::Asset

#endif
