#include "Anim.hpp"

Anim::Anim(void) : Tex() { _init(); }

Anim::Anim(SDL_Renderer *renderer, const std::string &src)
    : Tex(renderer, src) {
  _init();
}

Anim::Anim(SDL_Renderer *renderer, const std::string &src, int frameSize)
    : Tex(renderer, src, frameSize) {
  _init();
}

Anim::Anim(Anim const &other) : Tex(other) {
  _paused = other._paused;
  _playing = other._playing;
  _loop = other._loop;
  _fps = other._fps;
  _timePerFrame = other._timePerFrame;
  _prevFrameTime = other._prevFrameTime;
}

Anim::~Anim(void) {}

Anim &Anim::operator=(const Anim &other) {
  if (this == &other) {
    return *this;
  }
  if (_tex)
    unload();
  _src = other._src;
  _renderer = other._renderer;
  _frame = other._frame;
  _currentFrame = other._currentFrame;
  _nFrame = other._nFrame;
  _width = other._width;
  _height = other._height;
  _framesPerRow = other._framesPerRow;
  _framesPerColumn = other._framesPerColumn;
  load(_src);
  _paused = other._paused;
  _playing = other._playing;
  _loop = other._loop;
  _fps = other._fps;
  _timePerFrame = other._timePerFrame;
  _prevFrameTime = other._prevFrameTime;
  return (*this);
}

void Anim::_init(void) {
  _paused = false;
  _playing = false;
  _loop = false;
  _fps = 12;
  _timePerFrame = std::chrono::milliseconds(1000 / _fps);
  _prevFrameTime = std::chrono::high_resolution_clock::now();
}

void Anim::start(void) {
  _prevFrameTime = std::chrono::high_resolution_clock::now();
  selectFrame(0);
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

bool Anim::isLooping(void) const { return _loop; }

void Anim::setLoop(bool loop) { _loop = loop; }

void Anim::setFps(int fps) {
  _fps = fps;
  _timePerFrame = std::chrono::milliseconds(1000 / _fps);
}

void Anim::draw(int x, int y) {
  if (!_playing)
    return;
  auto currentTime = std::chrono::high_resolution_clock::now();
  if (currentTime - _prevFrameTime >= _timePerFrame) {
    if (_currentFrame < _nFrame - 1)
      selectFrame(_currentFrame + 1);
    else {
      selectFrame(0);
      if (!_loop)
        stop();
    }
    _prevFrameTime = currentTime;
  }
  Tex::draw(x, y);
}

void Anim::drawRot(int x, int y, float angle) {
  if (!_playing)
    return;
  auto currentTime = std::chrono::high_resolution_clock::now();
  if (currentTime - _prevFrameTime >= _timePerFrame) {
    if (_currentFrame < _nFrame - 1)
      selectFrame(_currentFrame + 1);
    else {
      selectFrame(0);
      if (!_loop)
        stop();
    }
    _prevFrameTime = currentTime;
  }
  Tex::drawRot(x, y, angle);
}
