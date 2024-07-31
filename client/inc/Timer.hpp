#ifndef TIMER_HPP
#define TIMER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

class Timer {
public:
  Timer(void);

  void start(void);
  void stop(void);
  void pause(void);
  void unpause(void);

  Uint32 getTicks(void) const;
  bool isStarted(void) const;
  bool isPaused(void) const;

private:
  Uint32 _startTicks;
  Uint32 _pausedTicks;
  bool _paused;
  bool _started;
};

class TimerFps {
public:
  TimerFps(void);
  TimerFps(float target);

  void setFpsTarget(float target);
  void capFps(void);
  void start(void);
  float getFps(void) const;

private:
  bool _started;
  float _target;
  float _fps;
  Uint32 _startTicks;
  Uint32 _frameCount;
  Uint32 _fpsUpdateInterval;
  Uint32 _targetFrameTime;
  Uint32 _lastFrameTicks;
};

#endif
