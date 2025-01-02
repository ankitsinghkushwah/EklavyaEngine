//
//  AudioComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 17/03/2024.
//

#ifndef AudioComponent_hpp
#define AudioComponent_hpp

#include "TransformComponent.h"
#include <glm/glm.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>

namespace Eklavya
{
	struct Audio
	{
	  public:
		Audio();
		void Load(std::array<std::string, 4> soundNames);
		void Update(const glm::vec3& position, float deltaTime);

		sf::Sound& GetSound()
		{
			return mSound;
		}
		void Play(const std::string& key, bool loop = true);

	  private:
		const TransformComponent*                        mTransform = nullptr;
		std::unordered_map<std::string, sf::SoundBuffer> mSoundMap;
		sf::Sound                                        mSound;
	};
} // namespace Eklavya

#endif /* AudioComponent_hpp */
