#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Bcrypt.h"
#include <iostream>


int screen_w = 576;
int screen_h = 800;

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
		_x -= 70.0f * dTime;
		if (_x < -screen_w) {
			_x = 0.0f;
		}
	}

	float x() {
		return _x;
	}
};

class Engine : public olc::PixelGameEngine {

	Bird* bird;
	Background* bg;
	Ground* ground;

	olc::Decal* sprites[5];

	olc::Decal* bg_image;
	olc::Decal* ground_image;



public:
	Engine() {
		sAppName = "Flappy Bird NEAT";
	}

	bool OnUserCreate() override {

		bg_image = new olc::Decal(new olc::Sprite("./Images/bg.png"));
		ground_image = new olc::Decal(new olc::Sprite("./Images/base.png"));

		sprites[0] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));
		sprites[1] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
		sprites[2] = new olc::Decal(new olc::Sprite("./Images/bird3.png"));
		sprites[3] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
		sprites[4] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));

		bird = new Bird(100.0f, 0.0f, sprites);
		bg = new Background();
		ground = new Ground();

		return true;
	}

	bool OnUserUpdate(float dTime) override {

		bird->Jump();
		bird->Gravity(dTime);
		bg->Move(dTime);
		ground->Move(dTime);


		DrawDecal({ round(bg->x()), -125 }, bg_image, { 2,2 });
		DrawDecal({ (round(bg->x()) + screen_w), -125 }, bg_image, { 2,2 });

		DrawDecal({ ground->x(), 675 }, ground_image, { 2,2 });
		DrawDecal({ ground->x() + screen_w, 675 }, ground_image, { 2,2 });

		DrawDecal({ round(bird->x()), round(bird->y()) }, bird->Animate(dTime), { 2,2 });


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