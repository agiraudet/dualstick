#ifndef ANIM_HPP
#define ANIM_HPP

#include "Tex.hpp"

#include <chrono>

class Anim {
public:
  Anim(Tex &tex);
  Anim(Anim const &other);
  ~Anim(void);
  Anim &operator=(const Anim &other);

  void start(void);
  void stop(void);
  void pause(void);
  void unpause(void);
  bool isPlaying(void) const;
  bool isPaused(void) const;
  bool isStopped(void) const;
  bool isLooping(void) const;
  void setLoop(bool loop);
  void setFps(int fps);

  void draw(int x, int y);
  void drawRot(int x, int y, float angle);

private:
  void _init(void);
  void _nextFrame(void);

private:
  Tex &_tex;
  bool _paused;
  bool _playing;
  bool _loop;
  int _fps;
  std::chrono::milliseconds _timePerFrame;
  std::chrono::high_resolution_clock::time_point _prevFrameTime;
};

#endif
