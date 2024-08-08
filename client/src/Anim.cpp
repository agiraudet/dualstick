#include "Anim.hpp"

Anim::Anim(Tex &tex)
    : _tex(tex), _paused(false), _playing(false), _loop(false), _fps(12) {
  _init();
}

Anim::Anim(Anim const &other)
    : _tex(other._tex), _paused(other._paused), _playing(other._playing),
      _loop(other._loop), _fps(other._fps), _timePerFrame(other._timePerFrame),
      _prevFrameTime(other._prevFrameTime) {}

Anim::~Anim(void) {}

Anim &Anim::operator=(const Anim &other) {
  if (this == &other) {
    return *this;
  }
  /*_tex = other._tex;*/
  _paused = other._paused;
  _playing = other._playing;
  _loop = other._loop;
  _fps = other._fps;
  _timePerFrame = other._timePerFrame;
  _prevFrameTime = other._prevFrameTime;
  return (*this);
}

void Anim::_init(void) {
  _timePerFrame = std::chrono::milliseconds(1000 / _fps);
  _prevFrameTime = std::chrono::high_resolution_clock::now();
}

void Anim::start(void) {
  _prevFrameTime = std::chrono::high_resolution_clock::now();
  _tex.selectFrame(0);
  _paused = false;
  _playing = true;
}

void Anim::stop(void) {
  _playing = false;
  _paused = false;
}

void Anim::pause(void) {
  if (_playing) {
    _paused = true;
    _playing = false;
  }
}

void Anim::unpause(void) {
  if (!_playing && _paused) {
    _prevFrameTime = std::chrono::high_resolution_clock::now();
    _playing = true;
    _paused = false;
  }
}

bool Anim::isPlaying(void) const { return _playing; }

bool Anim::isPaused(void) const { return _paused; }

bool Anim::isStopped(void) const { return !(_paused || _playing); }

bool Anim::isLooping(void) const { return _loop; }

void Anim::setLoop(bool loop) { _loop = loop; }

void Anim::setFps(int fps) {
  _fps = fps;
  _timePerFrame = std::chrono::milliseconds(1000 / _fps);
}

void Anim::draw(int x, int y) {
  _nextFrame();
  _tex.draw(x, y);
}

void Anim::drawRot(int x, int y, float angle) {
  _nextFrame();
  _tex.drawRot(x, y, angle);
}

void Anim::_nextFrame(void) {
  if (!_playing)
    return;
  auto currentTime = std::chrono::high_resolution_clock::now();
  if (currentTime - _prevFrameTime >= _timePerFrame) {
    int currentFrame = _tex.getCurrentFrame();
    if (currentFrame < _tex.getNFrame() - 1)
      _tex.selectFrame(currentFrame + 1);
    else {
      _tex.selectFrame(0);
      if (!_loop)
        stop();
    }
    _prevFrameTime = currentTime;
  }
}
