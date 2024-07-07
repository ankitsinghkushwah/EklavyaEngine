//
//  Main.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 27/05/2023.
//

#include "Director.hpp"
#include "SkeletalAnimationDemo.hpp"
#include "VehiclePhysicsDemo.hpp"
#include "FPSDemo.hpp"
#include "TPSDemo.hpp"
#include "StackOfBoxesDemo.hpp"

using namespace Eklavya;

enum Resolution {
  LOW,    // 1024x576
  HD,     // 1280x720
  FULL_HD // 1920x1080
};

struct ResolutionInfo {
  int width;
  int height;
};

constexpr ResolutionInfo resolutions[] = {
    {1024, 576}, // LOW
    {1280, 720}, // HD
    {1920, 1080} // FULL HD
};

int main() {
  ResolutionInfo resolution = resolutions[Resolution::FULL_HD];

  Eklavya::Director *engine = new Eklavya::Director(
      "Eklavya", resolution.width, resolution.height,false);
  engine->SetScene(new StackOfBoxesDemo(engine));
  engine->Start();
  return 0;
}
