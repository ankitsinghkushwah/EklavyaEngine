#ifndef INPUT_HANDLER
#define INPUT_HANDLER

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class UserInputListener;

enum JOYSTICK_INPUT_ID
{
	HORIZONTAL_AXIS = 0,
	VERTICAL_AXIS = 1,
	RIGHT_TRIGGER = 4,
	LEFT_TRIGGER = 5
};

class InputHandler
{
  public:
	static InputHandler* GetInstance()
	{
		static InputHandler* instance = new InputHandler();
		return instance;
	}

	InputHandler();

	void Update(float dt);
	void OnMouseAction(int key, int action);
	void OnKeyAction(int key, int action);
	void OnCursorMove(double x, double y);
	void OnControllerStateChanged(int, int);

	void AddListener(UserInputListener* pListener);
	void RemoveListener(UserInputListener* pListener);

	inline bool KeyHasPressed(int key) { return mKeyStates[key]; }

	inline float GetMouseX() { return mMousePos.x; }

	inline float GetMouseY() { return mMousePos.y; }

	inline glm::vec2 GetMouseDragDelta() const
	{
		if (mDragging == false)
			return glm::vec2(0.0f);
		else
			return mMousePos - mLastMousePos;
	}

	inline bool IsControllerConnected() { return mControllerReady; }

	// Controller
	float GetAxis(int axisId);

	void ImGuiProc();

	void VibrateController();

	~InputHandler();

  private:
	std::vector<UserInputListener*> mListners;
	bool                            mKeyStates[1024];
	glm::vec2                       mMousePos;
	glm::vec2                       mLastMousePos;

	bool  mDragging = false;
	float mTimePassedSinceLastDrag = 0.0f;

	// controller inputs
	bool mControllerReady = false;
};

#endif
