#include "Timer.hpp"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>

Timer::Timer(void)
    : _startTicks(0), _pausedTicks(0), _paused(false), _started(false) {}

void Timer::start(void) {
  _started = true;
  _paused = false;
  _startTicks = SDL_GetTicks();
  _pausedTicks = 0;
}

void Timer::stop(void) {
  _started = false;
  _paused = false;
  _startTicks = 0;
  _pausedTicks = 0;
}

void Timer::pause(void) {
  if (_started && !_paused) {
    _paused = true;
    _pausedTicks = SDL_GetTicks() - _startTicks;
    _startTicks = 0;
  }
}

void Timer::unpause(void) {
  if (_started && _paused) {
    _paused = false;
    _startTicks = SDL_GetTicks() - _pausedTicks;
    _pausedTicks = 0;
  }
}

Uint32 Timer::getTicks(void) const {
  if (!_started)
    return 0;
  if (_paused)
    return _pausedTicks;
  return SDL_GetTicks() - _startTicks;
}

bool Timer::isStarted(void) const { return _started; }

bool Timer::isPaused(void) const { return _started && _paused; }

// FPS TIMER

TimerFps::TimerFps(void) : _countedFrame(0), _target(30.f), _startTime(0) {
  _tickPerFrame = 1000 / _target;
}

TimerFps::TimerFps(float target)
    : _countedFrame(0), _target(target), _tickPerFrame(0.f), _startTime(0) {
  _tickPerFrame = 1000 / _target;
}

void TimerFps::setFpsTarget(float target) {
  _target = target;
  _tickPerFrame = 1000 / _target;
}

void TimerFps::capFps(void) {
  Uint32 time = SDL_GetTicks();
  if (_startTime == 0)
    _startTime = SDL_GetTicks();
  ++_countedFrame;
  if (time - _lastFrame < _tickPerFrame)
    SDL_Delay(time - _lastFrame);
}

float TimerFps::getFps(void) const {
  return (SDL_GetTicks() - (double)_startTime) / _countedFrame * 100;
}
