#ifndef INC_JOINT
#define INC_JOINT

/* Container for bone data */

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <list>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct KeyPosition {
  glm::vec3 position;
  float timeStamp;
};

struct KeyRotation {
  glm::quat orientation;
  float timeStamp;
};

struct KeyScale {
  glm::vec3 scale;
  float timeStamp;
};

class Bone {
public:
  Bone() {}
  Bone(const std::string &name, int ID, const aiNodeAnim *channel);
  ~Bone();
  void Update(float animationTime);
  glm::mat4 GetLocalTransform() { return m_LocalTransform; }
  std::string GetJointName() const { return m_Name; }
  int GetJointID() { return m_ID; }
  std::vector<Bone *> GetChildren() { return m_Children; }
  void AddChild(Bone *child);
  inline void SetParent(Bone *parent) { m_Parent = parent; }
  inline const Bone *GetParent() { return m_Parent; }
  void PrintHeirarchy();

  glm::vec3 GetPosAtIndex(int index) { return m_Positions[index].position; }
  glm::quat GetRotAtIndex(int index) { return m_Rotations[index].orientation; }
  glm::vec3 GetScaleAtIndex(int index) { return m_Scales[index].scale; }

  int GetPositionIndex(float animationTime);
  int GetRotationIndex(float animationTime);
  int GetScaleIndex(float animationTime);

private:
  float GetScaleFactor(float lastTimeStamp, float nextTimeStamp,
                       float animationTime);
  glm::mat4 InterpolatePosition(float animationTime);
  glm::mat4 InterpolateRotation(float animationTime);
  glm::mat4 InterpolateScaling(float animationTime);

  std::vector<KeyPosition> m_Positions;
  std::vector<KeyRotation> m_Rotations;
  std::vector<KeyScale> m_Scales;
  int m_NumPositions;
  int m_NumRotations;
  int m_NumScalings;

  glm::mat4 m_LocalTransform;
  std::string m_Name;
  std::vector<Bone *> m_Children;
  Bone *m_Parent;
  int m_ID;
};

#endif
