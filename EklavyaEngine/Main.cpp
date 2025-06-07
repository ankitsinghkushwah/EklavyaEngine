//
//  Main.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 27/05/2023.
//

#include <Director.hpp>
#include <rang.hpp>
#include <Shadows/ShadowsDemo.h>
#include <FrustumCulling/FrustumCullingDemo.h>
#include <VehiclePhysicsDemo/VehiclePhysicsDemo.hpp>
#include <SkeletalAnimationWithIK/SkeletalAnimationWithIK.hpp>
#include <StackOfBoxes/StackOfBoxesDemo.hpp>
#include <vector>
#include <filesystem>
#include "SkeletalAnimation/SkeletalAnimationDemo.hpp"
#include <imgui/imgui.h>


using namespace Eklavya;
using namespace std;
using namespace rang;

enum Resolution
{
	LOW, // 1024x576
	HD, // 1280x720
	FULL_HD // 1920x1080
};

struct ResolutionInfo
{
	int width;
	int height;
};


constexpr ResolutionInfo resolutions[] = {
	{1024, 576}, // LOW
	{1280, 720}, // HD
	{1920, 1080} // FULL HD
};

void SetWorkingDirectoryToExecutablePath(char *argv[])
{
	// Get the path of the executable
	std::filesystem::path executablePath = std::filesystem::canonical(std::filesystem::path(argv[0]));
	std::filesystem::path executableDir = executablePath.parent_path();

	// Change the working directory to the executable's directory
	if (std::filesystem::exists(executableDir) && std::filesystem::is_directory(executableDir))
	{
		std::filesystem::current_path(executableDir);
		std::cout << "Working directory set to: " << executableDir << std::endl;
	}
	else
	{
		std::cerr << "Failed to set working directory!" << std::endl;
	}
}

int main(int argc, char *argv[])
{
	SetWorkingDirectoryToExecutablePath(argv);

	ResolutionInfo resolution = resolutions[Resolution::FULL_HD];
	Director director = Director("Eklavya", resolution.width, resolution.height, false);

	director.Start();

	std::filesystem::path currentPath = std::filesystem::current_path();
	std::cout << style::italic << "Current working directory: " << currentPath << std::endl;

	return 0;
}
