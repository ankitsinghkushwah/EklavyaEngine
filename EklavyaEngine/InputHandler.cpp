#include "InputHandler.h"
#include <GLFW/glfw3.h>
#include "UserInputListener.h"
#include <imgui/imgui.h>
#include <ForceFeedback/ForceFeedback.h>

InputHandler::InputHandler() : mMousePos(0.0f), mLastMousePos(0.0f)
{

  for (int index = 0; index < 1024; ++index)
    mKeyStates[index] = false;

  mListners.reserve(10);
}

void InputHandler::Update(float dt)
{
  if (mDragging)
    {
      mTimePassedSinceLastDrag += dt;
      if (mTimePassedSinceLastDrag > 0.1f)
        {
          mTimePassedSinceLastDrag = 0.0f;
          mDragging = false;
        }
    }
  for (auto listener : mListners)
    listener->PollKeyAction();

  if (mControllerReady == false)
    {
      mControllerReady = glfwJoystickPresent(GLFW_JOYSTICK_1) == 1;
    }
}

void InputHandler::OnMouseAction(int key, int action)
{
  for (auto listener : mListners)
    listener->OnMouseAction(key, action);
}

void InputHandler::OnKeyAction(int key, int action)
{
  if (action == GLFW_PRESS)
    mKeyStates[key] = true;
  if (action == GLFW_RELEASE)
    mKeyStates[key] = false;

  for (auto listener : mListners)
    listener->OnKeyAction(key, action);
}

void InputHandler::OnCursorMove(double x, double y)
{
  mDragging = true;
  mLastMousePos = mMousePos;
  mMousePos = glm::vec2(x, y);
  for (auto listener : mListners)
    listener->OnCursorMove(x, y);
}

void InputHandler::OnControllerStateChanged(int joy, int event) {}

void InputHandler::AddListener(UserInputListener *pListener)
{
  mListners.push_back(pListener);
}

void InputHandler::RemoveListener(UserInputListener *pListener)
{
  auto iter = std::find(mListners.begin(), mListners.end(), pListener);
  if (iter != mListners.end())
    mListners.erase(iter); //"Iter" cannot be used because Its invalidated
}

void  InputHandler::VibrateController() {}

float InputHandler::GetAxis(int axisId)
{
  int          axesCount = 0;
  const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
  return axes[axisId];
}

void InputHandler::ImGuiProc()
{
  if (mControllerReady)
    {
      ImGui::Begin("Controller");

      int          axesCount;
      const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

      for (int i = 0; i < axesCount; i++)
        {
          ImGui::Text("\n Axis : %.3f", axes[i]);
        }

      ImGui::End();
    }
}

InputHandler::~InputHandler() { mListners.clear(); }
