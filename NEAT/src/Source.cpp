#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int screen_w = 576;
int screen_h = 800;

class Bird {
	float _x;
	float _y;
	

	float animationTime;
	float frameTime = 1.0 / 6.0;

	olc::Sprite* sprites[5];
	olc::Sprite* current_image;

	float fallTime = 0.0;
	float vel = 0.0;
	float g = 9.82;
	


public:
	Bird(float x, float y, olc::Sprite* sprt[5]) {
		_x = x;
		_y = y;
		animationTime = 0.0;

		for (int i = 0; i < 5; i++) {
			sprites[i] = sprt[i];
		}

		current_image = new olc::Sprite("./Images/bird1.png");
	}
public:
	olc::Sprite* Animate(float dTime) {

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

		float s = (vel * t) + (g * (t * t) / 2.0);

		if (s > 3.7) {
			s = 3.7;
		}

		if (s < 0.0) {
			s -= 2.0;
		}
		s = s*300.0 * (dTime);

		_y += s;

		
		//Temp
		if (_y > screen_h) {
			fallTime = 0.0;
			_y = 0.0;
			vel = 0.0;
		}
	}
	void Jump() {
		if (_y > screen_h / 2.0 && _y < screen_h / 2.0 + 10.0) {
			fallTime = 0.0;
			vel = -1.3;
		}
	}

	float x() {
		return _x;
	}
	float y() {
		return _y;
	}
};

class Engine : public olc::PixelGameEngine {

	Bird* bird;
	olc::Sprite* sprites[5] = { new olc::Sprite("./Images/bird1.png"),
								new olc::Sprite("./Images/bird2.png"),
								new olc::Sprite("./Images/bird3.png"),
								new olc::Sprite("./Images/bird2.png"),
								new olc::Sprite("./Images/bird1.png") };

public:
	Engine() {
		sAppName = "Flappy Bird NEAT";
	}

	bool OnUserCreate() override {

		bird = new Bird(100.0, 0.0, sprites);
		return true;
	}

	bool OnUserUpdate(float dTime) override {

		bird->Jump();
		bird->Gravity(dTime);

		Clear(olc::BLACK);
		DrawSprite({ (int)round(bird->x()), (int)round(bird->y()) }, bird->Animate(dTime), 2, 0);

		return true;
	}
};

int main()
{
	Engine engine;
	if (engine.Construct(screen_w, screen_h, 1, 1))
		engine.Start();

	return 0;
}