#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#define RandomY (600.0f - abs(Random() % 361))
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286f

const int screen_w = 576;
const int screen_h = 800;

const int scale = 2;
int pipe_w;
int pipe_h;
int bird_w;
int bird_h;

int score = 0;
bool game_loop = false;
float degrees = PI / 180;
float t = 0.0f;


struct Bird {
	float x = 0.0f;
	float y = 0.0f;
	std::vector <olc::Decal*> sprites;

	olc::Sprite* current{};
	float animationTime{};
	float frameTime = 1.0f / 17.0f;

	float vel{};
	float g = 9.82f;
	bool collide{};

	float s{};
	float height{};
	float angle{};

	//by is currently not needed
	float bx{};
	float by{};

	olc::Decal* Animate(float dTime) {

		animationTime += dTime;
		if (animationTime >= (5.0 * frameTime)) {
			animationTime = 0.0;
		}

		current = sprites[(int)(animationTime / frameTime)]->sprite;
		return sprites[(int)(animationTime / frameTime)];
	}


	void Gravity(float dTime) {
		if (dTime > 0.2f) { dTime = 0.2f; }

		s = 300 * vel * dTime;
		vel += (g / 2) * dTime;

		if (s > 3.7) {
			s = 3.7f;
		}

		y += s;

		//Temp
		if (y > screen_h) {
			y = 0.0;
			vel = 0.0;
			angle = 0;
			height = 0;
		}
	}

	void Rotate(float dTime) {
		float maxRotation = -27.0f * degrees;
		float rotVel = 0.8f * degrees;

		if (height < y) {
			angle += rotVel * 400 * (dTime);
		}

		if (s < 0) {
			angle = maxRotation;
		}
		else if ((90 * degrees) < angle) {
			angle = 90 * degrees;
		}

	}
	void Jump() {
		vel = -1.67;
		height = y - bird_h / 4;
	}
};



struct Pipe {
	float x{};
	float y{};
	bool newPipe = true;
	bool hasNotScored = true;

	void Move(float dTime) {
		x -= 220.0f * (dTime);
	}

	void Score(float birdX) {
		if (x < birdX and hasNotScored) {
			score++;
			hasNotScored = false;
		}
	}
};



struct Scenery {
	float x{};
	float speed{};

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

	std::vector<std::vector<bool>> mask_PipeImg_Down;
	std::vector<std::vector<bool>> mask_PipeImg_Up;
	std::vector<std::vector<bool>> mask_Ground;

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

		//Tint is temp
		tint = *new olc::Pixel(255, 255, 255);

		bird = { 50.0f, 80.0f, sprites };
		background = { 0.0f, 20.0f };
		ground = { 0.0f, 220.0f };

		pipeList.push_back(Pipe{ (float)screen_w, RandomY });

		pipe_w = scale * pipeImg_Down->sprite->width;
		pipe_h = scale * pipeImg_Down->sprite->height;
		bird_w = scale * sprites[0]->sprite->width;
		bird_h = scale * sprites[0]->sprite->height;

		mask_PipeImg_Down = GetMask(pipeImg_Down->sprite);
		mask_PipeImg_Up = GetMask(pipeImg_Up->sprite);
		mask_Ground = GetMask(ground_image->sprite);

		return true;
	}


	bool OnUserUpdate(float dTime) override {
		Actions(dTime);
		Draw(dTime);
		return true;
	}

	//###################################################################################################################################

	void Actions(float dTime) {

		if (game_loop) {

			bird.Gravity(dTime);
			bird.Rotate(dTime);

			for (Pipe& pipe : pipeList) {
				pipe.Move(dTime);
				pipe.Score(bird.bx);
			}
		}

		ground.Move(dTime);
		background.Move(dTime);

		//Tools
		DevTools(dTime);
		t += dTime;
		//

		NewPossiblePipe();
		PipeOffScreen();

		Collision();
		//Temp
		if (bird.collide) {
			tint.r = 255;
			tint.g = 0;
			tint.b = 0;
		}
		else {
			tint.r = 255;
			tint.g = 255;
			tint.b = 255;
		}
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


	void DevTools(float dTime) {             //some tools
		float speed = 100.0f * dTime;

		if (GetKey(olc::Key::UP).bHeld) {
			bird.y -= speed;
		}
		else if (GetKey(olc::Key::RIGHT).bHeld) {
			bird.x += speed;
		}
		else if (GetKey(olc::Key::DOWN).bHeld) {
			bird.y += speed;
		}
		else if (GetKey(olc::Key::LEFT).bHeld) {
			bird.x -= speed;
		}
		else if (GetKey(olc::Key::SPACE).bPressed) {
			bird.Jump();
		}
		else if (GetKey(olc::Key::R).bPressed) {
			bird.x = pipeList.front().x;
			bird.y = pipeList.front().y;
			bird.vel = 0.0f;
			bird.angle = 0;
		}
		else if (GetKey(olc::Key::G).bPressed) {
			if (game_loop) {
				game_loop = false;
			}
			else if (!game_loop) {
				game_loop = true;
			}
		}
		else if (GetKey(olc::Key::E).bHeld) {
			bird.angle += -25 * degrees * 40 * (dTime);
		}
		else if (GetKey(olc::Key::Q).bHeld) {
			bird.angle += 25 * degrees * 40 * (dTime);
		}
	}


	void Collision() {
		bird.collide = false;

		float x = (abs(cos(bird.angle)) * (bird_w / (2.0f))) - (abs(sin(bird.angle)) * (-bird_h / (2.0f)));
		float y = (abs(sin(bird.angle)) * (-bird_w / (2.0f))) + (abs(cos(bird.angle)) * (-bird_h / (2.0f)));

		bird.by = bird.y + (bird_h / 2.f) + y;
		bird.bx = bird.x + (bird_w / 2.f) - x;

		float by = bird.by;
		float bx = bird.bx;

		std::vector<std::vector<bool>> mask_Bird;

		if ((by + abs(y + y)) > 675) {
			mask_Bird = RotateBirdMask();
			PixelPerfect(mask_Ground, mask_Bird, (675 - (int)by), 0);
			if (bird.collide) return;
		}


		for (Pipe& pipe : pipeList) {
			float py = pipe.y;
			float px = pipe.x;

			if (((bx < px + pipe_w) and (px < (bx + abs(x + x)))) and (((by + abs(y + y)) > py) or (py - (screen_h - pipe_h) > by))) {

				if (mask_Bird.empty()) {
					mask_Bird = RotateBirdMask();
				}

				PixelPerfect(mask_PipeImg_Down, mask_Bird, (int)round(pipe.y) - (int)round(by), (int)round(pipe.x) - (int)round(bx));
				if (bird.collide) return;
				PixelPerfect(mask_PipeImg_Up, mask_Bird, (int)round(pipe.y - screen_h) - (int)round(by), (int)round(pipe.x) - (int)round(bx));
				if (bird.collide) return;
				if ((by < 0) and (bx < px + pipe_w - 2 * scale) and ((px + 2 * scale) < (bx + abs(x + x)))) {
					bird.collide = true;
					return;
				}

			}
		}
	}


	void PixelPerfect(std::vector<std::vector<bool>> mask, std::vector<std::vector<bool>> mask_Bird, int dY, int dX) {

		int _y = std::max(0, -dY);
		int h = std::min((int)mask.size(), (int)(mask_Bird.size() - dY));

		int _x = std::max(0, -dX);
		int w = std::min((int)mask[0].size(), (int)(mask_Bird[0].size() - dX));

		if (_x < w and _y < h) {
			for (int y = _y; y < h; y++) {
				for (int x = _x; x < w; x++) {
					if (mask_Bird[y + dY][x + dX] and mask[y][x]) {
						bird.collide = true;
						return;
					}
					else {
						bird.collide = false;
					}
				}
			}
		}
	}


	std::vector<std::vector<bool>> GetMask(olc::Sprite* sprite) {

		const int h = sprite->height * scale;
		const int w = sprite->width * scale;

		std::vector<std::vector<bool>> mask(h, std::vector<bool>(w, false));
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				mask[y][x] = (sprite->Sample((float)x / (float)w, (float)y / (float)h).a == 255);
			}
		}
		return mask;
	}


	std::vector<std::vector<bool>> RotateBirdMask() {
		float s = sin(bird.angle);
		float c = cos(bird.angle);

		int w = abs(2 * ((int)round((abs(c) * (bird_w / (2.0f))) - (abs(s) * (-bird_h / (2.0f))))));
		int h = abs(2 * ((int)round((abs(s) * (-bird_w / (2.0f))) + (abs(c) * (-bird_h / (2.0f))))));

		auto mask = GetMask(bird.current);
		std::vector<std::vector<bool>> rotated_mask(h, std::vector<bool>(w, false));

		int dx = (int)round(w / 2.f - bird_w / 2.f);
		int dy = (int)round(h / 2.f - bird_h / 2.f);

		//Can reverse the procces to get rid of the gaps
		for (int y = 0; y < bird_h; y++) {
			for (int x = 0; x < bird_w; x++) {

				int rx = (int)round((c * (x - bird_w / 2.0f)) - (s * (y - bird_h / (2.0f)))) + (w / 2);
				int ry = (int)round((s * (x - bird_w / (2.0f))) + (c * (y - bird_h / (2.0f)))) + (h / 2);

				if (rx < 0) rx = 0;
				if (rx >= w) rx = w - 1;
				if (ry < 0) ry = 0;
				if (ry >= h) ry = h - 1;

				rotated_mask[ry][rx] = mask[y][x];
			}
		}
		return rotated_mask;
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

		//float x1 = (abs(cos(bird.angle)) * (bird_w / (2.0f))) - (abs(sin(bird.angle)) * (-bird_h / (2.0f)));
		//float y1 = (abs(sin(bird.angle)) * (-bird_w / (2.0f))) + (abs(cos(bird.angle)) * (-bird_h / (2.0f)));

		//FillRectDecal({ round(bird.x + (bird_w / 2) - x1), round(bird.y + (bird_h / 2) + y1) }, { abs(x1 + x1), abs(y1 + y1) }, *new olc::Pixel(255, 0, 125, 155));
		DrawRotatedDecal({ round(bird.x + (bird_w / 2)), round(bird.y + (bird_h / 2)) }, bird.Animate(dTime), bird.angle, { bird_w / (2.0f * scale), bird_h / (2.0f * scale) }, { scale,scale }, tint);

		DrawDecal({ g_x, 675 }, ground_image, { scale,scale });
		DrawDecal({ g_x + screen_w, 675 }, ground_image, { scale,scale });

		DrawStringDecal({ TextCenter() - 3, 103 }, std::to_string(score), olc::BLACK, { 3.01f * scale, 4.1f * scale });
		DrawStringDecal({ TextCenter(), 100 }, std::to_string(score), olc::WHITE, { 3 * scale, 4 * scale });
	}


	int64_t Random() {
		int64_t value;
		BYTE buffer[sizeof(INT64_MAX)];
		DWORD size = sizeof(INT64_MAX);

		BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		std::memcpy(&value, buffer, 8);

		return value;
	}


	float TextCenter() {
		int digit = 0;

		for (char i : std::to_string(score)) {
			digit++;
		}
		return ((screen_w - digit * (8 * 3 * scale)) / 2.f);
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


