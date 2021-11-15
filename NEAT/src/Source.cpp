#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib, "bcrypt.lib")


int screen_w = 576;
int screen_h = 800;
int pipe_w = 102;
int pipe_h = 640;

bool g_bool = false;
float t = 0.0f;


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
	bool collide = false;

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
			s *= 11.1f;
			s -= 1.0f;
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
	void Jump(float dTime) {
		//if (_y > screen_h / 2.0 && _y < screen_h / 2.0 + 50.0) {
		fallTime = 0.0;
		vel = -0.85f;
		//}
	}

	float x() {
		return _x;
	}
	void sx(float x) {
		_x = x;
	}
	float y() {
		return _y;
	}
	void sy(float y) {
		_y = y;
	}
	void svel(float v) {
		vel = v;
		fallTime = v;
	}
	bool hit() {
		return collide;
	}
	void shit(bool boo) {
		collide = boo;
	}
};

struct _Pipe{
	float x;
	float y;
	bool newPipe;

	void Move(float dTime) {
		x = x - 220.0f * (dTime);
	}
	

};

class Pipe {

public:
	float _x;
	float _y;

private:
	bool _newPipe = true;

public:
	Pipe(float x, float y) {
		_x = x;
		_y = y;
	}

	void Move(float dTime) {
		_x = _x - 220.0f * (dTime);
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

struct Moving {
	float x;
	float speed;

	void Move(float dTime) {
		x -= speed * dTime;
		if (x < -screen_w) {
			x = 0.0f;
		}
	}
};

class Engine : public olc::PixelGameEngine {
	std::vector<Pipe*> pipeList;

	Bird* bird;
	Moving background;
	Moving ground;

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

		bird = new Bird(50.0f, 80.0f, sprites);
		background = { 0.0f, 20.0f };
		ground = { 0.0f, 220.0f };

		pipeList.push_back(new Pipe((float)screen_w, 600.0f - (float)(abs(Random()) % 361)));
		//pipeList.push_back(new Pipe(50.0f, 600.0f - 360.0f));
		//pipeList.push_back(new Pipe(440.0f, 600.0f - 0.0f));

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
		//bird->Jump();
		if (g_bool) {
			bird->Gravity(dTime);
			for (Pipe* pipe : pipeList) {
				pipe->Move(dTime);
			}
		}

		//back->Move(dTime); 
		ground.Move(dTime);
		background.Move(dTime);

		DevMove(dTime);
		t += dTime;

		Collide();

		NewPossiblePipe();
		PipeOffScreen();
	}

	void NewPossiblePipe() {
		Pipe* pipe = pipeList.front();
		float x = pipe->x();

		if (x < (screen_w / 2) && (pipe->newPipe() == true)) {
			pipeList.push_back(new Pipe((x + (float)pipe_w) + screen_w / 2.0f, 600.0f - (float)(abs(Random()) % 361)));
			pipe->AllowNewPipe(false);
		}
	}

	void PipeOffScreen() {
		if (pipeList.front()->x() < -pipe_w) {
			pipeList.erase(pipeList.begin());
		}
	}

	void DevMove(float dTime) {
		float speed = 100.0f * dTime;


		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			bird->sy(bird->y() - speed);
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			bird->sx(bird->x() + speed);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			bird->sy(bird->y() + speed);
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			bird->sx(bird->x() - speed);
		}
		else if (GetAsyncKeyState(VK_SPACE) & 0x0101) {
			bird->Jump(dTime);
		}
		else if (GetAsyncKeyState('R') & 0x0101) {
			bird->sx(50.0f + pipe_w);
			bird->sy(600.0f - 48.0f);
			bird->svel(0.0f);

			//pipeList.clear();
			//pipeList.push_back(new Pipe(50, 600.0f - 0.0f));
			//pipeList.push_back(new Pipe(440, 600.0f - 360.0f));
		}
		else if (GetAsyncKeyState('G') & 0x0101) {
			if (g_bool) { //&& t > 0.3) {
				g_bool = false;
				//		t = 0.0f;
			}
			else if (!g_bool) {// && t > 0.1) {
				g_bool = true;
				//		t = 0.0f;
				//	}
			}
			if (bird->y() <= 601 && bird->y() >= 600) {
				//std::cout << pipeList.back()->x() << std::endl;
			}
		}
	}

	void Collide() {
		float by = bird->y();
		float bx = bird->x();

		for (Pipe* pipe : pipeList) {
			float py = pipe->y();
			float px = pipe->x();

			std::cout << (((px <= bx) and (bx <= (px + pipe_w))) and ((py <= by) or ((py - 160) >= by))) << std::endl;

			if ((px <= bx && bx <= (px + pipe_w)) && (!(py <= by) && !(by <= (py + pipe_h)))) {
				bird->shit(true);
				std::cout << "Collide" << std::endl;
			}
			else { bird->shit(false); }
			//std::cout << bird->hit() << std::endl;
		}
		std::cout << std::endl;
	}

	void Draw(float dTime) {
		float bg_x = round(background.x);
		float g_x = round(ground.x);

		DrawDecal({ bg_x, -125 }, bg_image, { 2,2 });
		DrawDecal({ (bg_x + screen_w), -125 }, bg_image, { 2,2 });

		for (Pipe* pipe : pipeList) {
			float y = round(pipe->y());
			float x = round(pipe->x());

			DrawDecal({ x, y }, pipeImg_Down, { 2,2 });
			DrawDecal({ x, (y - 800) }, pipeImg_Up, { 2,2 });
		}

		if (bird->hit()) {
			//SetPixelMode(olc::Pixel::MASK);
			DrawDecal({ round(bird->x()), round(bird->y()) }, bird->Animate(dTime), { 2,2 }, *new olc::Pixel(255, 0, 0, 180));
			//SetPixelMode(olc::Pixel::NORMAL);
		}
		else {
			DrawDecal({ round(bird->x()), round(bird->y()) }, bird->Animate(dTime), { 2,2 });
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