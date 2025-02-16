#ifndef INC_APPLICATION_H
#define INC_APPLICATION_H
// C++ headers
#include <string>
// third party headers
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vector>

class GLWindowContext
{
  public:
	GLWindowContext();
	~GLWindowContext();
	bool               Create(const std::string& name, int majorVersion, int minorVersion, int width, int height, bool fullScreen = false);
	void               Destroy();
	void               SetPosition(int x, int y);
	void               SetDimension(int width, int height);
	inline int         GetMajorVersion() const { return m_MajorGLVersion; }
	inline int         GetMinorVersion() const { return m_MinorGLVersion; }
	inline float       GetWidth() const { return m_Width; }
	inline float       GetHeight() const { return m_Height; }
	inline std::string GetName() { return m_Name; }
	inline GLFWwindow* GetWindow() const { return m_Window; }
	inline float       GetAspectRatio() const { return m_Width / float(m_Height); }
	void               SwapBuffers();

  private:
	int         m_X;
	int         m_Y;
	int         m_MinorGLVersion;
	int         m_MajorGLVersion;
	int         m_Width;
	int         m_Height;
	bool        m_IsFullScreen;
	std::string m_Name;
	GLFWwindow* m_Window;
};
class GLFWGame
{
  private:
	bool InitImGUI();

  protected:
	GLWindowContext* m_CurrentContext;

  public:
	GLFWGame();
	virtual ~GLFWGame();
	virtual bool            Initialize(const std::string& pName, int pWindowWidth, int pWindowHeight, bool pFullScreen);
	virtual void            Destroy();
	virtual void            Tick() {}
	virtual void            ImGuiProc();
	void                    Start();
	void                    GameLoop();
	inline GLWindowContext* GetCurrentContext() { return m_CurrentContext; }
	// Callbacks
	virtual void OnKeyAction(int pKey, int pAction) = 0;
	virtual void OnError(int pError, const char* pDesc) = 0;
	virtual void OnResize(int newWidth, int newHeight) = 0;
	virtual void OnMouseAction(int pKey, int pAction) = 0;
	virtual void OnCursorMoved(double pX, double pY) = 0;
	virtual void OnJoystickStateChange(int joy, int event) = 0;
	void         CloseWindow();
	void         HideMouse(bool hide = true);
};
class CallbackManager
{
  public:
	static void SetCallbacks(GLFWGame* pApp);

  private:
	static GLFWGame* mApp;
	static void      KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void      ErrorCallback(int error, const char* desc);
	static void      WindowResizeCallback(GLFWwindow* window, int width, int height);
	static void      MouseButtonCallback(GLFWwindow* window, int key, int action, int mods);
	static void      CursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
	static void      JoystickCallback(int jid, int event);
};
#endif
