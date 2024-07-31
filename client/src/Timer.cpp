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

///////////////
//           //
// FPS TIMER //
//           //
///////////////

TimerFps::TimerFps(void)
    : _started(false), _target(60.f), _fps(0.f), _startTicks(0), _frameCount(0),
      _fpsUpdateInterval(1000), _lastFrameTicks(0) {
  _targetFrameTime = 1000.f / _target;
}

TimerFps::TimerFps(float target)
    : _started(false), _target(target), _fps(0.f), _startTicks(0),
      _frameCount(0), _fpsUpdateInterval(1000), _lastFrameTicks(0) {
  _targetFrameTime = 1000.f / _target;
}

void TimerFps::setFpsTarget(float target) {
  _target = target;
  _targetFrameTime = 1000.f / _target;
}

void TimerFps::start(void) {
  if (!_started) {
    _started = true;
    _startTicks = SDL_GetTicks();
    _lastFrameTicks = _startTicks;
  }
}

void TimerFps::capFps(void) {
  if (!_started)
    start();
  Uint32 currentTicks = SDL_GetTicks();
  Uint32 elapsedTicks = currentTicks - _startTicks;
  if (elapsedTicks >= _fpsUpdateInterval) {
    _fps = _frameCount / (elapsedTicks / 1000.0f);
    _startTicks = currentTicks;
    _frameCount = 0;
  }
  Uint32 frameTime = currentTicks - _lastFrameTicks;
  _lastFrameTicks = currentTicks;
  if (frameTime < _targetFrameTime)
    SDL_Delay(_targetFrameTime - frameTime);
  _frameCount++;
}

float TimerFps::getFps(void) const { return _fps; }
