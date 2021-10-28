#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Bcrypt.h"
#include <iostream>
#include <random>


int screen_w = 576;
int screen_h = 800;

std::random_device rd;

class Bird {
  float _x;
  float _y;

  float animationTime;
  float frameTime = 1.0f / 6.0f;

  olc::Decal* sprites[5];
  olc::Decal* current_image;

  float fallTime = 0.0;
  float vel = 0.0;
  float g = 9.82f;

public:
  Bird(float x, float y, olc::Decal* sprt[5]) {
    _x = x;
    _y = y;
    animationTime = 0.0;

    for (int i = 0; i < 5; i++) {
      sprites[i] = sprt[i];
    }

    current_image = sprt[1];
  }

  olc::Decal* Animate(float dTime) {

    animationTime += dTime;
    if (animationTime >= (5.0 * frameTime)) {
      animationTime = 0.0;
    }

    current_image = sprites[(int)(animationTime / frameTime)];
    return current_image;
  }

  void Gravity(float dTime) {
    fallTime += dTime;
    float t = fallTime;

    float s = (vel * t) + (g * (t * t) / 2.0f);

    if (s > 3.7) {
      s = 3.7f;
    }

    if (s < 0.0) {
      s -= 2.0;
    }
    s = s * 300.0f * (dTime);

    _y += s;


    //Temp
    if (_y > screen_h) {
      fallTime = 0.0;
      _y = 0.0;
      vel = 0.0;
    }
  }
  void Jump() {
    if (_y > screen_h / 2.0 && _y < screen_h / 2.0 + 50.0) {
      fallTime = 0.0;
      vel = -1.3f;
    }
  }

  float x() {
    return _x;
  }
  float y() {
    return _y;
  }
};

class Pipe {

  float _x;
  float _y;
  olc::Decal* _pipe;
  bool _newPipe = true;

public:
  Pipe(float x, float y, olc::Decal* pipe) {
    _x = x;
    _y = y;
    _pipe = pipe;
  }

  void Move(float dTime) {
    _x = _x - 150.0f * (dTime);
  }

  float x() {
    return _x;
  }

  float y() {
    return _y;
  }

  bool newPipe() {
    return _newPipe;
  }

  void SetNewPipe(bool boolean) {
    _newPipe = boolean;
  }

  olc::Decal* pipe() {
    return _pipe;
  }
};

class Background {

  float _x;

public:
  Background() {
    _x = 0.0f;
  }

  void Move(float dTime) {
    _x -= 12.0f * dTime;
    if (_x < -screen_w) {
      _x = 0.0f;
    }
  }
  float x() {
    return _x;
  }
};

class Ground {
  float _x;

public:
  Ground() {
    _x = 0.0f;
  }

  void Move(float dTime)
  {
    _x -= 150.0f * dTime;
    if (_x < -screen_w) {
      _x = 0.0f;
    }
  }

  float x() {
    return _x;
  }
};


class Engine : public olc::PixelGameEngine {
  std::vector<Pipe*> pipeList;

  Bird* bird;
  Background* bg;
  Ground* ground;

  olc::Decal* sprites[5];

  olc::Decal* bg_image;
  olc::Decal* ground_image;

  olc::Decal* pipeI;

public:
  Engine() {
    sAppName = "Flappy Bird NEAT";
  }

  bool OnUserCreate() override {

    bg_image = new olc::Decal(new olc::Sprite("./Images/bg.png"));
    ground_image = new olc::Decal(new olc::Sprite("./Images/base.png"));
    pipeI = new olc::Decal(new olc::Sprite("./Images/pipe.png"));

    sprites[0] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));
    sprites[1] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
    sprites[2] = new olc::Decal(new olc::Sprite("./Images/bird3.png"));
    sprites[3] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
    sprites[4] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));

    bird = new Bird(100.0f, 0.0f, sprites);
    bg = new Background();
    ground = new Ground();

    pipeList.push_back(new Pipe(screen_w, 700.0 - (rd() % 440), pipeI));

    return true;
  }

  bool OnUserUpdate(float dTime) override {

    bird->Jump();
    bird->Gravity(dTime);
    bg->Move(dTime);
    ground->Move(dTime);


    DrawDecal({ round(bg->x()), -125 }, bg_image, { 2,2 });
    DrawDecal({ (round(bg->x()) + screen_w), -125 }, bg_image, { 2,2 });

    DrawDecal({ round(bird->x()), round(bird->y()) }, bird->Animate(dTime), { 2,2 });


    for (Pipe* pipe : pipeList) {
      pipe->Move(dTime);
    }
    if (pipeList.front()->x() < (screen_w / 2) && (pipeList.front()->newPipe() == true)) {
      pipeList.push_back(new Pipe((pipeList.front()->x() + 102) + screen_w / 2, 600.0 - (rd() % 400), pipeI));
      pipeList.front()->SetNewPipe(false);
    }if (pipeList.front()->x() < -102) {
      pipeList.erase(pipeList.begin());
    }

    //Clear(olc::BLACK);
    for (Pipe* pipe : pipeList) {
      DrawDecal({ round(pipe->x()),round(pipe->y()) }, pipe->pipe(), {2,2});
      DrawDecal({ round(pipe->x()),round(pipe->y() - 800) }, pipe->pipe(), {2,2});
    }

    DrawDecal({ ground->x(), 675 }, ground_image, { 2,2 });
    DrawDecal({ ground->x() + screen_w, 675 }, ground_image, { 2,2 });
    return true;
  }
};

int main()
{

  Engine engine;
  if (engine.Construct(screen_w, screen_h, 1, 1)) {
    engine.Start();
  }

  return 0;
}