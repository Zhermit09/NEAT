#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Bcrypt.h"
#include <iostream>
#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib, "bcrypt.lib")


int screen_w = 576;
int screen_h = 800;
int pipe_w = 102;

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

		current_image = sprt[0];
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
	bool _newPipe = true;

public:
	Pipe(float x, float y) {
		_x = x;
		_y = y;
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

	void AllowNewPipe(bool boolean) {
		_newPipe = boolean;
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
	olc::Decal* pipeImg_Down;
	olc::Decal* pipeImg_Up;

public:
	Engine() {
		sAppName = "Flappy Bird NEAT";
	}

	bool OnUserCreate() override {

		bg_image = new olc::Decal(new olc::Sprite("./Images/bg.png"));
		ground_image = new olc::Decal(new olc::Sprite("./Images/base.png"));
		pipeImg_Down = new olc::Decal(new olc::Sprite("./Images/pipe.png"));
		pipeImg_Up = new olc::Decal(new olc::Sprite("./Images/pipe1.png"));

		sprites[0] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));
		sprites[1] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
		sprites[2] = new olc::Decal(new olc::Sprite("./Images/bird3.png"));
		sprites[3] = new olc::Decal(new olc::Sprite("./Images/bird2.png"));
		sprites[4] = new olc::Decal(new olc::Sprite("./Images/bird1.png"));

		bird = new Bird(100.0f, 0.0f, sprites);
		bg = new Background();
		ground = new Ground();

		pipeList.push_back(new Pipe((float)screen_w, 600.0f - (float)(abs(Random()) % 400)));

		return true;
	}

	bool OnUserUpdate(float dTime) override {

		Action(dTime);
		Draw(dTime);

		return true;
	}

	int Random() {
		int value;
		BYTE buffer[sizeof(INT_MAX)];
		DWORD size = sizeof(INT_MAX);

		BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		std::memcpy(&value, buffer, 4);

		return value;
	}

	void Action(float dTime) {
		bird->Jump();
		bird->Gravity(dTime);

		bg->Move(dTime);
		ground->Move(dTime);

		for (Pipe* pipe : pipeList) {
			pipe->Move(dTime);
		}

		NewPossiblePipe();
		PipeOffScreen();
	}

	void NewPossiblePipe() {
		Pipe* pipe = pipeList.front();
		float x = pipe->x();

		if (x < (screen_w / 2) && (pipe->newPipe() == true)) {
			pipeList.push_back(new Pipe((x + (float)pipe_w) + screen_w / 2.0f, 600.0f - (float)(abs(Random()) % 400)));
			pipe->AllowNewPipe(false);
		}
	}

	void PipeOffScreen() {
		if (pipeList.front()->x() < -pipe_w) {
			pipeList.erase(pipeList.begin());
		}
	}

	void Draw(float dTime) {
		float bg_x = round(bg->x());
		float g_x = ground->x();

		DrawDecal({ bg_x, -125 }, bg_image, { 2,2 });
		DrawDecal({ (bg_x + screen_w), -125 }, bg_image, { 2,2 });

		DrawDecal({ round(bird->x()), round(bird->y()) }, bird->Animate(dTime), { 2,2 });

		for (Pipe* pipe : pipeList) {
			float y = round(pipe->y());
			float x = round(pipe->x());

			DrawDecal({ x, y }, pipeImg_Down, { 2,2 });
			DrawDecal({ x, (y - 800) }, pipeImg_Up, { 2,2 });
		}

		DrawDecal({ g_x, 675 }, ground_image, { 2,2 });
		DrawDecal({ g_x + screen_w, 675 }, ground_image, { 2,2 });
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