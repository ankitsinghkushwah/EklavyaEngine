#ifndef _INC_HIP_HOP_INPUT_MANAGER_H_
#define _INC_HIP_HOP_INPUT_MANAGER_H_

#include <InputHandler.h>

/* will only be inherited by pawn actors*/
class UserInputListener
{
  public:
	virtual void PollKeyAction()
	{
	}

	virtual void OnMouseAction(int key, int action)
	{
	}

	virtual void OnKeyAction(int key, int action)
	{
	}

	virtual void OnCursorMove(double x, double y)
	{
	}

	UserInputListener()
	{
		InputHandler::GetInstance()->AddListener(this);
	}

	virtual ~UserInputListener()
	{
		InputHandler::GetInstance()->RemoveListener(this);
	}

	inline bool IsKeyPressed(int key)
	{
		return InputHandler::GetInstance()->KeyHasPressed(key);
	}
};

#endif
