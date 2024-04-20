#include "GameActor.h"
#include "BaseComponent.h"
#include "../Utils/Logger.h"

GameActor::GameActor(const std::string &name, ActorID id)
    : m_Name(name), m_ID(id) {
  m_Children.reserve(10);
}

GameActor::~GameActor() {}

bool GameActor::Init() {
  for (ComponentMap::iterator iter = m_Components.begin();
       iter != m_Components.end(); ++iter) {
    iter->second->Init();
  }
  return false;
}

void GameActor::Destroy() {
  LOG_STRING("\nDestroying " + m_Name + "...");
  for (ComponentMap::iterator iter = m_Components.begin();
       iter != m_Components.end(); ++iter) {
    iter->second->Destroy();
  }

  m_Components.clear();
}

void GameActor::Tick(float deltaTime) {
  for (ComponentMap::iterator iter = m_Components.begin();
       iter != m_Components.end(); ++iter) {
    iter->second->Tick(deltaTime);
  }
}

void GameActor::AddComponent(std::shared_ptr<BaseComponent> NewComponent) {
  ComponentID id = NewComponent->GetID();
  m_Components[id] = NewComponent;
}

void GameActor::RemoveComponent(ComponentID id) {
  ComponentMap::iterator position = m_Components.find(id);
  m_Components.erase(position);
}

void GameActor::AddChild(std::shared_ptr<GameActor> pChild) {
  m_Children.push_back(pChild);
}

void GameActor::RemoveChild(std::shared_ptr<GameActor> pChild) {
  auto iter = std::find(m_Children.begin(), m_Children.end(), pChild);
  if (iter != m_Children.end())
    m_Children.erase(iter);
}
