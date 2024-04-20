#ifndef INC_COLLIDER_H_
#define INC_COLLIDER_H_

#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Eklavya::Physics
{
  class EkBody;

  enum class EColliderType
  {
    BOX = 0,
    SPHERE,
    MAX
  };

  class ICollider
  {
  public:
    ICollider(EColliderType type);
    virtual ~ICollider();

    EColliderType           GetType() { return m_Type; }

    inline void             SetBody(EkBody *pBody) { mBody = pBody; }

    inline EkBody          *GetBody() { return mBody; }

    inline const glm::vec3 &GetOffset() { return m_Offset; }

    inline void SetOffset(const glm::vec3 &offset) { m_Offset = offset; }

    void        SetGroupIndex(int groupIndex) { mGroupFlag = groupIndex; }

    inline int GroupFlag() { return mGroupFlag; }

  protected:
    int      mGroupFlag;
    glm::vec3     m_Offset;
    EColliderType m_Type;
    EkBody       *mBody = nullptr;
  };

  class SphereCollider : public ICollider
  {
  public:
    SphereCollider();
    ~SphereCollider();

    float GetRadius() { return mRadius; }

    void  SetRadius(float radius);

  private:
    float mRadius;
  };

  class BoxCollider : public ICollider
  {
  public:
    BoxCollider();
    ~BoxCollider();

    glm::vec3 GetHalfSize() { return m_HalfSize; }

    void      SetHalfSize(glm::vec3 halfSize);

  private:
    glm::vec3 m_HalfSize;
  };

  struct PlaneCollider
  {
    glm::vec3 m_Normal;
    float     d;
  };
} // namespace Eklavya::Physics
#endif
