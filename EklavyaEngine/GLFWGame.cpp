
#include <glad/glad.h>
#include "GLFWGame.h"
#include <stdio.h>
#include "CoreUtils/Logger.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <sstream>
#include <iomanip>
#include <Timer.h>

// GLWindowContext
GLWindowContext::GLWindowContext() : m_Window(nullptr)
{
}

GLWindowContext::~GLWindowContext()
{
}

bool GLWindowContext::Create(const std::string& name, int majorVersion, int minorVersion, int width, int height, bool fullScreen)
{
	m_Name = name;
	m_X = m_Y = 0;
	m_Width = width;
	m_Height = height;
	m_MajorGLVersion = majorVersion;
	m_MinorGLVersion = minorVersion;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 32);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_Window = glfwCreateWindow(width, height, name.c_str(), fullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (!m_Window)
	{
		fprintf(stderr, "\n failed to create glfw window");
		return false;
	}
	Timer::GetInstance().Update();
	return true;
}

void GLWindowContext::SetPosition(int x, int y)
{
	if (m_Window)
		glfwSetWindowPos(m_Window, x, y);
}

void GLWindowContext::SetDimension(int width, int height)
{
	if (m_Window)
		glfwSetWindowSize(m_Window, width, height);
}

void GLWindowContext::Destroy()
{
	if (m_Window)
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}
}

void GLWindowContext::SwapBuffers()
{
	glfwSwapBuffers(m_Window);
}

GLFWGame::GLFWGame()
{
}

// GLFWGame
bool GLFWGame::InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_CurrentContext->GetWindow(), true);

	if (!ImGui_ImplOpenGL3_Init("#version 150"))
	{
		Logger::GetInstance().LogToConsole("failed to init IMGUI");
		return false;
	}
	io.Fonts->AddFontDefault();
	return true;
}

void GLFWGame::ImGuiProc()
{
	ImGui::Begin("STATS"); // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

bool GLFWGame::Initialize(const std::string& pName, int pWndWidth, int pWndHeight, bool pFullScreen)
{
	if (!glfwInit())
	{
		fprintf(stderr, "\n Failed to initialize GLFW");
		return false;
	}

	m_CurrentContext = new GLWindowContext();
	bool success = m_CurrentContext->Create(pName, 4, 3, pWndWidth, pWndHeight, pFullScreen);
	if (!success)
		return false;
	glfwMakeContextCurrent(m_CurrentContext->GetWindow());

	CallbackManager::SetCallbacks(this);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Glad : Failed to load glLoader");
		return false;
	}
#ifndef DISABLE_IMGUI
	if (!InitImGUI())
	{
		printf("failed to init ImGui");
	}
#endif

	return true;
}

void GLFWGame::Destroy()
{
	if (m_CurrentContext)
	{
		delete m_CurrentContext;
		m_CurrentContext = nullptr;
	}
	glfwTerminate();
}

GLFWGame::~GLFWGame()
{
}

void GLFWGame::HideMouse(bool hide)
{
	glfwSetInputMode(m_CurrentContext->GetWindow(), GLFW_CURSOR, hide ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GLFWGame::CloseWindow()
{
	glfwSetWindowShouldClose(m_CurrentContext->GetWindow(), GL_TRUE);
}

void GLFWGame::Start()
{
	GameLoop();
}

void GLFWGame::GameLoop()
{
	while (true)
	{
		glfwPollEvents();
		if (!glfwWindowShouldClose(m_CurrentContext->GetWindow()))
		{
			this->Tick();

#ifndef DISABLE_IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			this->ImGuiProc();
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(m_CurrentContext->GetWindow(), &display_w, &display_h);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
#endif
			m_CurrentContext->SwapBuffers();
		}
		else
			break;
	}
}

void CallbackManager::SetCallbacks(GLFWGame* pApp)
{
	mApp = pApp;
	GLFWwindow* window = mApp->GetCurrentContext()->GetWindow();
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetJoystickCallback(JoystickCallback);
}

void CallbackManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (mApp)
		mApp->OnKeyAction(key, action);
}

void CallbackManager::ErrorCallback(int error, const char* desc)
{
	if (mApp)
		mApp->OnError(error, desc);
}

void CallbackManager::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	if (mApp)
		mApp->OnResize(width, height);
}

void CallbackManager::MouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
{
	if (mApp)
		mApp->OnMouseAction(key, action);
}

void CallbackManager::CursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (mApp)
		mApp->OnCursorMoved(xPos, yPos);
}

void CallbackManager::JoystickCallback(int jid, int event)
{
	if (mApp)
		mApp->OnJoystickStateChange(jid, event);
}

GLFWGame* CallbackManager::mApp = nullptr;
