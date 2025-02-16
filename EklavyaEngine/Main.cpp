//
//  Main.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 27/05/2023.
//

#include <Director.hpp>
#include <rang.hpp>
#include <SkeletalAnimationWithIK/SkeletalAnimationWithIK.hpp>
#include <vector>
#include <filesystem>

using namespace Eklavya;
using namespace std;
using namespace rang;

enum Resolution
{
	LOW,    // 1024x576
	HD,     // 1280x720
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

void SetWorkingDirectoryToExecutablePath(char* argv[])
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

int main(int argc, char* argv[])
{
	SetWorkingDirectoryToExecutablePath(argv);

	std::filesystem::path currentPath = std::filesystem::current_path();
	std::cout << style::italic << "Current working directory: " << currentPath << std::endl;

	std::vector<std::string> demos =
	    {"Deferred Pipeline : Shadows", "Fructum Culling", "Skeletal Animation", "Skeletal Animation With IK", "Vehicle Physics Demo", "Custom Physics - Stack Of Boxes"};

	ResolutionInfo resolution = resolutions[Resolution::FULL_HD];
	cout << style::bold << rang::fg::cyan << "=================EKLAVYA ENGINE DEMOS ====================\n\n";
	int idx = 0;
	for (const std::string& demo : demos)
	{
		cout << style::bold << rang::fg::green << ++idx <<". "<< demo;
		cout << "\n";
	}

	cout << style::bold << rang::fg::cyan << "\n\n";

	int user_selection = 0;
	cout << "Enter number between 1 to " << demos.size() << " to launch demo\n";
	cin >> user_selection;

	cout << style::reset << rang::fg::reset;

	Eklavya::Director* engine = new Eklavya::Director("Eklavya", resolution.width, resolution.height, false);
	engine->SetScene(new SkeletalAnimationWithIK(engine));
	engine->Start();
	return 0;
}
