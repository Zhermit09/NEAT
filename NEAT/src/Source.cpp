#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <random>


int screen_w = 576;
int screen_h = 800;

std::random_device rd;

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
		s = s * 300.0 * (dTime);

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

class Pipe {

	float _x;
	float _y;
	olc::Sprite* _pipe;
	bool _newPipe = true;

public:
	Pipe(float x, float y, olc::Sprite* pipe) {
		_x = x;
		_y = y;
		_pipe = pipe;
	}

	void Move(float dTime) {
		_x = _x - 300.0 * (dTime);
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

	olc::Sprite* pipe() {
		return _pipe;
	}
};


class Engine : public olc::PixelGameEngine {
	std::vector<Pipe*> pipeList;
	Bird* bird;

	olc::Sprite* sprites[5] =
	{
		new olc::Sprite("./Images/bird1.png"),
		new olc::Sprite("./Images/bird2.png"),
		new olc::Sprite("./Images/bird3.png"),
		new olc::Sprite("./Images/bird2.png"),
		new olc::Sprite("./Images/bird1.png")
	};
	olc::Sprite* pipeI = new olc::Sprite("./Images/pipe.png");

public:
	Engine() {
		sAppName = "Flappy Bird NEAT";
	}

	bool OnUserCreate() override {

		pipeList.push_back(new Pipe(screen_w, 700.0 - (rd() % 440), pipeI));
		bird = new Bird(100.0, 0.0, sprites);

		return true;
	}

	bool OnUserUpdate(float dTime) override {

		bird->Jump();
		bird->Gravity(dTime);
		for (Pipe* pipe : pipeList) {
			pipe->Move(dTime);
		}
		if (pipeList.front()->x() < (screen_w / 2) && (pipeList.front()->newPipe() == true)) {
			pipeList.push_back(new Pipe((pipeList.front()->x() + 102) + screen_w / 2, 700.0 - (rd() % 440), pipeI));
			pipeList.front()->SetNewPipe(false);
		}

		if (pipeList.front()->x() < -102) {
			pipeList.erase(pipeList.begin());
		}

		Clear(olc::BLACK);
		DrawSprite({ (int)round(bird->x()), (int)round(bird->y()) }, bird->Animate(dTime), 2, 0);
		for (Pipe* pipe : pipeList) {
			DrawSprite({ (int)round(pipe->x()),(int)round(pipe->y()) }, pipe->pipe(), 2, 0);
			DrawSprite({ (int)round(pipe->x()),(int)round(pipe->y() - 840) }, pipe->pipe(), 2, 2);
		}
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