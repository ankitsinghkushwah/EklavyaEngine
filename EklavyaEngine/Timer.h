//
// Created by Ankit Singh on 11-11-2018.
//

#ifndef _INC_TIMER_HPP
#define _INC_TIMER_HPP

#include <chrono>
#include "CoreUtils/Singleton.h"

using Clock = std::chrono::steady_clock;
using TimeStamp = std::chrono::steady_clock::time_point;

class Timer : public Singleton<Timer> {
public:
  Timer();
  ~Timer();

  inline double GetDeltaTimeInMS() { return mFrameTime; }

  inline float GetFrameTime() { return mFrameTime * .001f; }

  inline int GetFramesPerSecond() { return m_FPS; }

  void Update();
  void Reset();

private:
  void UpdateDelta();
  void UpdateFPS();

  // for frame delta
  TimeStamp mLastTime;
  double mFrameTime;

  // for fps
  double m_ElapsedTime;
  int m_FPSCounter;
  int m_FPS;
};

#endif // BURNOUTENGINE_Timer_HPP
