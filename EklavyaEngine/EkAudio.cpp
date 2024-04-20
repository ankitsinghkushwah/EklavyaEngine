//
//  Audio.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 17/03/2024.
//

#include "EkAudio.hpp"
#include "TransformComponent.h"
#include "EkActor.h"

const std::string SOUND_DIR = "Resources/sounds/";

namespace Eklavya {
Audio::Audio() {}

void Audio::Load(std::array<std::string, 4> sounds) {
  for (int idx = 0; idx < 1; ++idx) {
    mSoundMap[sounds[idx]] = sf::SoundBuffer();
    mSoundMap[sounds[idx]].loadFromFile(SOUND_DIR + sounds[idx]);
  }
}

void Audio::Play(const std::string &key, bool loop) {
  mSound.setBuffer(mSoundMap[key]);
  mSound.setLoop(loop);
  mSound.play();
}

void Audio::Update(const glm::vec3 &pos, float deltaTime) {
  mSound.setPosition(pos.x, pos.y, pos.z);
}

} // namespace Eklavya
