//
// Created by Ankit Singh on 11-11-2018.
//
#include "Timer.h"

using namespace std;
using namespace std::chrono_literals;

Timer::Timer()
    : Singleton(), mFrameTime(0.0f), m_ElapsedTime(0.0f),
      mLastTime(Clock::now()) {}

Timer::~Timer() {}

void Timer::Update() {
  UpdateDelta();
  UpdateFPS();
}

void Timer::UpdateDelta() {
    TimeStamp now = Clock::now();
  mFrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                   (now - mLastTime))
                   .count();
  mLastTime = now;
}

void Timer::UpdateFPS() {
  m_ElapsedTime += GetFrameTime();
  m_FPSCounter++;
  if (m_ElapsedTime > 1.0f) {
    m_FPS = m_FPSCounter;
    m_ElapsedTime = 0.0f;
    m_FPSCounter = 0;
  }
}
