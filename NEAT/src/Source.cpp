#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#define RandomY (600.0f - abs(Random() % 361))

const int screen_w = 576;
const int screen_h = 800;

const int scale = 2;
int pipe_w;
int pipe_h;
int bird_w;
int bird_h;

bool g_bool = false;
float t = 0.0f;


struct Bird {
	float x = 0.0f;
	float y = 0.0f;
	std::vector <olc::Decal*> sprites;

	float animationTime = 0.0f;
	float frameTime = 1.0f / 6.0f;

	float fallTime = 0.0f;
	float vel = 0.0f;
	float g = 9.82f;
	bool collide = false;

	olc::Decal* Animate(float dTime) {

		animationTime += dTime;
		if (animationTime >= (5.0 * frameTime)) {
			animationTime = 0.0;
		}

		return sprites[(int)(animationTime / frameTime)];
	}


	void Gravity(float dTime) {
		fallTime += dTime;
		float t = fallTime;

		float s = (vel * t) + (g * (t * t) / 2.0f);
		if (s > 3.7) {
			s = 3.7f;
		}

		if (s < 0.0f) {
			s *= 11.1f;
			s -= 1.0f;
		}
		y += s * 300.0f * (dTime);


		//Temp
		if (y > screen_h) {
			fallTime = 0.0;
			y = 0.0;
			vel = 0.0;
		}
	}

	void Jump(float dTime) {
		fallTime = 0.0;
		vel = -0.85f;
	}
};



struct Pipe {
	float x;
	float y;
	bool newPipe = true;

	void Move(float dTime) {
		x -= 220.0f * (dTime);
	}

};



struct Scenery {
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
	std::vector<Pipe> pipeList;
	std::vector<olc::Decal*> sprites;

	Bird bird;
	olc::Pixel tint;
	Scenery background;
	Scenery ground;

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

		sprites = {
			new olc::Decal(new olc::Sprite("./Images/bird1.png")),
			new olc::Decal(new olc::Sprite("./Images/bird2.png")),
			new olc::Decal(new olc::Sprite("./Images/bird3.png")),
			new olc::Decal(new olc::Sprite("./Images/bird2.png")),
			new olc::Decal(new olc::Sprite("./Images/bird1.png"))
		};

		tint = *new olc::Pixel(255, 255, 255);
		bird = { 50.0f, 80.0f, sprites };
		background = { 0.0f, 20.0f };
		ground = { 0.0f, 220.0f };

		pipeList.push_back(Pipe{ (float)screen_w, RandomY });

		pipe_w = scale * pipeImg_Down->sprite->width;
		pipe_h = scale * pipeImg_Down->sprite->height;
		bird_w = scale * sprites[0]->sprite->width;
		bird_h = scale * sprites[0]->sprite->height;

		return true;
	}


	bool OnUserUpdate(float dTime) override {

		Actions(dTime);
		Draw(dTime);

		return true;
	}

	//###################################################################################################################################

	void Actions(float dTime) {

		if (g_bool) {

			bird.Gravity(dTime);

			for (Pipe& pipe : pipeList) {
				pipe.Move(dTime);
			}
		}
		
		ground.Move(dTime);
		background.Move(dTime);

		//Tools
		DevMove(dTime);
		t += dTime;
		//

		Collide();

		NewPossiblePipe();
		PipeOffScreen();
	}


	void NewPossiblePipe() {
		Pipe pipe = pipeList.front();

		if (pipe.x < (screen_w / 2) && (pipe.newPipe == true)) {
			pipeList.push_back(Pipe{ (pipe.x + pipe_w) + (screen_w / 2), RandomY });

			//Must get new front pipe after push_back()
			pipeList.front().newPipe = false;
		}
	}

	void PipeOffScreen() {
		if (pipeList.front().x < -pipe_w) {
			pipeList.erase(pipeList.begin());
		}
	}

	void DevMove(float dTime) {             //some tools
		float speed = 100.0f * dTime;


		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			bird.y -= speed;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			bird.x += speed;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			bird.y += speed;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			bird.x -= speed;
		}
		else if (GetAsyncKeyState(VK_SPACE) & 0x0101) {
			bird.Jump(dTime);
		}
		else if (GetAsyncKeyState('R') & 0x0101) {
			bird.x = 50.0f + pipe_w;
			bird.y = pipeList.front().y - bird_h;
			bird.vel = 0.0f;

		}
		else if (GetAsyncKeyState('G') & 0x0101) {
			if (g_bool) { 
				g_bool = false;
			}
			else if (!g_bool) {
				g_bool = true;
			}
		}
	}

	void Collide() {
		float by = bird.y;
		float bx = bird.x;

		for (Pipe& pipe : pipeList) {
			float py = pipe.y;
			float px = pipe.x;

			if (((bx < px + pipe_w) and (px < (bx + bird_w))) and (((by + bird_h) > py) or (py - (screen_h - pipe_h) > by))) {
				bird.collide = true;
				tint.g = 0;
				tint.b = 0;
				break;
			}
			//Temp???
			else {
				bird.collide = false;
				tint.g = 255;
				tint.b = 255;
			}
		}
	}

	//###################################################################################################################################

	void Draw(float dTime) {

		float bg_x = round(background.x);
		float g_x = round(ground.x);

		DrawDecal({ bg_x, -125 }, bg_image, { scale,scale });
		DrawDecal({ (bg_x + screen_w), -125 }, bg_image, { scale,scale });

		for (Pipe& pipe : pipeList) {
			float y = round(pipe.y);
			float x = round(pipe.x);

			DrawDecal({ x, y }, pipeImg_Down, { scale,scale });
			DrawDecal({ x, (y - screen_h) }, pipeImg_Up, { scale,scale });
		}

		DrawDecal({ round(bird.x), round(bird.y) }, bird.Animate(dTime), { scale,scale }, tint);

		DrawDecal({ g_x, 675 }, ground_image, { scale,scale });
		DrawDecal({ g_x + screen_w, 675 }, ground_image, { scale,scale });
	}


	int Random() {
		int value;
		BYTE buffer[sizeof(INT_MAX)];
		DWORD size = sizeof(INT_MAX);

		BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		std::memcpy(&value, buffer, 4);

		return value;
	}
};


int main()
{
	int pixelSize = 1;

	Engine engine;
	if (engine.Construct(screen_w, screen_h, pixelSize, pixelSize)) {
		engine.Start();
	}

	return 0;
}