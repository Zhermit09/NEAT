#define OLC_PGE_APPLICATION
#define NEAT_AI_NEURALNETWORK
#include "olcPixelGameEngine.h"
#include "neat.h"
#include <Windows.h>
#include <Bcrypt.h>
//#include <thread>
#pragma comment(lib, "bcrypt.lib")
#define RandomY (600.0f - abs(Random() % 361))
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286f

const int screen_w = 576;
const int screen_h = 800;
const int true_screen_w = screen_w + 1000;
const int true_screen_h = screen_h + 0;

const int scale = 2;
int pipe_w;
int pipe_h;
int bird_w;
int bird_h;

int score = 0;
bool game_loop = false;  //can prob remove it as global later


int64_t Random() {
	int64_t value;
	BYTE buffer[sizeof(INT64_MAX)];
	DWORD size = sizeof(INT64_MAX);

	BCryptGenRandom(NULL, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	std::memcpy(&value, buffer, 8);

	return value;
}


struct Bird {
	float x{};
	float y{};
	std::vector <olc::Decal*> sprites;

	olc::Sprite* current{};
	float animationTime{};
	float frameTime = 1.0f / 17.0f;

	float vel{};
	float g = 9.82f;
	bool collide{};

	float s{};
	float jumpY{};
	float angle{};

	float bx{};
	float by{};

	Bird(float x, float y, std::vector <olc::Decal*> s) {
		this->x = x;
		this->y = y;
		sprites = s;
	}

	olc::Decal* Animate(float dTime) {

		animationTime += dTime;
		if (animationTime >= (5.0 * frameTime)) {
			animationTime = 0.0;
		}
		olc::Decal* frame = sprites[(int)(animationTime / frameTime)];
		current = frame->sprite;
		return frame;
	}

	int Gravity(float dTime) {
		s = 300 * vel * dTime;
		vel += (g / 2) * dTime;

		//Removed distance limmit

		y += s;
		return 0;
	}

	int Rotate(float dTime) {
		float degrees = PI / 180;
		float startAngle = -27.0f * degrees;
		float rotVel = 0.8f * degrees;

		if (jumpY < y) {
			angle += rotVel * 400 * (dTime);
		}

		if (angle > (90 * degrees)) {
			angle = 90 * degrees;
		}

		return 0;
	}

	int Jump() {
		vel = -1.66f;
		jumpY = y - bird_h / 3;
		angle = -27.0f * PI / 180;
		return 0;
	}
};


struct Pipe {
	float x{};
	float y{};

	int Move(float dTime) {
		x -= 220.0f * (dTime);
		return 0;
	}
};


struct Scenery {
	float x{};
	float speed{};

	int Move(float dTime) {
		x -= speed * dTime;
		if (x < -screen_w) {
			x = 0.0f;
		}
		return 0;
	}
};


class Engine : public olc::PixelGameEngine {
	int currentPipe;
	int scorePipe;
	std::vector<Pipe> pipeList;
	std::vector<olc::Decal*> sprites;

	std::vector<Bird*> birds;
	Scenery background;
	Scenery ground;

	olc::Decal* bg_image;
	olc::Decal* ground_image;
	olc::Decal* pipeImg_Down;
	olc::Decal* pipeImg_Up;

	std::vector<std::vector<bool>> mask_PipeImg_Down;
	std::vector<std::vector<bool>> mask_PipeImg_Up;
	std::vector<std::vector<bool>> mask_Ground;

	//neat::Network* firstAi;
	//neat::Net * no;
	neat::NEAT tean;

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

		pipe_w = scale * pipeImg_Down->sprite->width;
		pipe_h = scale * pipeImg_Down->sprite->height;
		bird_w = scale * sprites[0]->sprite->width;
		bird_h = scale * sprites[0]->sprite->height;

		for (int i = 0; i < neat::Population_Size; i++)
		{
			birds.push_back(new Bird(((screen_w / 2.f) - (2.f * bird_w)), (screen_h - 125) / 2.f, sprites));
		}
		background = { 0.0f, 20.0f };
		ground = { 0.0f, 220.0f };

		pipeList.push_back(Pipe{ (float)screen_w, RandomY });
		currentPipe = 0;
		scorePipe = 0;

		mask_PipeImg_Down = GetMask(pipeImg_Down->sprite);
		mask_PipeImg_Up = GetMask(pipeImg_Up->sprite);
		mask_Ground = GetMask(ground_image->sprite);


		//firstAi = new neat::Network();
		//no = new neat::Net(neat::Genome());
		tean = neat::NEAT();
		tean.Init();
		return true;
	}

	//###################################################################################################################################

	int GameLoop(float dTime) {
		//Cehck order later

		std::vector<Bird*> remove;
		std::vector<neat::Network*> rem;

		if (game_loop) {

			for (Bird* bird : birds) {
				bird->Gravity(dTime);
				bird->Rotate(dTime);
			}
			for (Pipe& pipe : pipeList) {
				pipe.Move(dTime);
			}
		}

		ground.Move(dTime);
		background.Move(dTime);

		DevTools(dTime);


		Score(birds[0]->bx);
		NewPossiblePipe();
		PipeOffScreen();

		AiTest();
		for (int i = 0; i < birds.size(); i++) {
			Collision(birds[i]);
			if (!birds[i]->collide) {
				remove.push_back(birds[i]);
				rem.push_back(tean.networks[i]);
			}
		}
		birds = remove;
		tean.networks = rem;
		GameOver();
		return 0;
	}

	int AiTest() {
		Pipe pipe = pipeList[currentPipe];

		if (game_loop) {

			for (int i = 0; i < birds.size(); i++)
			{
				auto result = tean.networks[i]->Activate({ birds[i]->by,  abs(pipeList[currentPipe].y - birds[i]->by), abs(pipeList[currentPipe].y - 161 - birds[i]->by) });

				if (result[0] > 0.5) {
					birds[i]->Jump();
				}
			}
		}

		if (score > 0)game_loop = false;

		return 0;
	}


	int Score(float birdX) {
		if (pipeList[scorePipe].x < birdX) {
			score++;
			scorePipe += 1;
		}
		return 0;
	}


	int NewPossiblePipe() {
		Pipe pipe = pipeList[currentPipe];

		if (pipe.x < (screen_w / 2)) {
			pipeList.push_back(Pipe{ (pipe.x + pipe_w) + (screen_w / 2), RandomY });
			currentPipe += 1;
		}
		return 0;
	}


	int PipeOffScreen() {
		if (pipeList.front().x < -pipe_w) {
			pipeList.erase(pipeList.begin());
			currentPipe -= 1;
			scorePipe -= 1;
		}
		return 0;
	}


	int DevTools(float dTime) {             //some tools
		float speed = 100.0f * dTime;
		float degrees = PI / 180;

		if (GetKey(olc::Key::UP).bHeld) {
			birds[0]->y -= speed;
		}
		else if (GetKey(olc::Key::RIGHT).bHeld) {
			birds[0]->x += speed;
		}
		else if (GetKey(olc::Key::DOWN).bHeld) {
			birds[0]->y += speed;
		}
		else if (GetKey(olc::Key::LEFT).bHeld) {
			birds[0]->x -= speed;
		}
		else if (GetKey(olc::Key::SPACE).bPressed) {
			birds[0]->Jump();
		}
		else if (GetKey(olc::Key::R).bPressed) {
			birds[0]->x = pipeList.front().x;
			birds[0]->y = pipeList.front().y;
			birds[0]->vel = 0.0f;
			birds[0]->angle = 0;
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
			birds[0]->angle += -25 * degrees * 10 * (dTime);
		}
		else if (GetKey(olc::Key::Q).bHeld) {
			birds[0]->angle += 25 * degrees * 10 * (dTime);
		}
		return 0;
	}


	int Collision(Bird* bird) {
		bird->collide = false;

		float x = (abs(cos(bird->angle)) * (bird_w / (2.0f))) - (abs(sin(bird->angle)) * (-bird_h / (2.0f)));
		float y = (abs(sin(bird->angle)) * (-bird_w / (2.0f))) + (abs(cos(bird->angle)) * (-bird_h / (2.0f)));

		bird->by = bird->y + (bird_h / 2.f) + y;
		bird->bx = bird->x + (bird_w / 2.f) - x;

		float by = bird->by;
		float bx = bird->bx;

		std::vector<std::vector<bool>> mask_Bird;

		if ((by + abs(y + y)) > 675) {
			mask_Bird = RotateBirdMask(bird);
			PixelPerfect(bird, mask_Ground, mask_Bird, (675 - (int)by), 0);
			if (bird->collide) return 0;
		}


		for (Pipe& pipe : pipeList) {
			float py = pipe.y;
			float px = pipe.x;

			if (((bx < px + pipe_w) and (px < (bx + abs(x + x)))) and (((by + abs(y + y)) > py) or (py - (screen_h - pipe_h) > by))) {

				if (mask_Bird.empty()) {
					mask_Bird = RotateBirdMask(bird);
				}

				PixelPerfect(bird, mask_PipeImg_Down, mask_Bird, (int)round(pipe.y) - (int)round(by), (int)round(pipe.x) - (int)round(bx));
				if (bird->collide) return 0;
				PixelPerfect(bird, mask_PipeImg_Up, mask_Bird, (int)round(pipe.y - screen_h) - (int)round(by), (int)round(pipe.x) - (int)round(bx));
				if (bird->collide) return 0;
				if ((by < 0) and (bx < px + pipe_w - 2 * scale) and ((px + 2 * scale) < (bx + abs(x + x)))) {
					bird->collide = true;
					return 0;
				}

			}
		}
		return 0;
	}


	int PixelPerfect(Bird* bird, std::vector<std::vector<bool>> mask, std::vector<std::vector<bool>> mask_Bird, int dY, int dX) {

		int _y = std::max(0, -dY);
		int h = std::min((int)mask.size(), (int)(mask_Bird.size() - dY));

		int _x = std::max(0, -dX);
		int w = std::min((int)mask[0].size(), (int)(mask_Bird[0].size() - dX));

		if (_x < w and _y < h) {
			for (int y = _y; y < h; y++) {
				for (int x = _x; x < w; x++) {
					if (mask_Bird[y + dY][x + dX] and mask[y][x]) {
						bird->collide = true;
						return 0;
					}
					else {
						bird->collide = false;
					}
				}
			}
		}
		return 0;
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


	std::vector<std::vector<bool>> RotateBirdMask(Bird* bird) {
		float s = sin(bird->angle);
		float c = cos(bird->angle);

		int w = abs(2 * ((int)round((abs(c) * (bird_w / (2.0f))) - (abs(s) * (-bird_h / (2.0f))))));
		int h = abs(2 * ((int)round((abs(s) * (-bird_w / (2.0f))) + (abs(c) * (-bird_h / (2.0f))))));

		auto mask = GetMask(bird->current);
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


	int GameOver() {
		if (birds.size() == 0) {
			for (int i = 0; i < neat::Population_Size; i++)
			{
				birds.push_back(new Bird(((screen_w / 2.f) - (2.f * bird_w)), (screen_h - 125) / 2.f, sprites));
			}
			pipeList.clear();
			pipeList.push_back(Pipe{ (float)screen_w, RandomY });
			score = 0;
			currentPipe = 0;
			scorePipe = 0;

			//no = new neat::Net(neat::Genome());
			tean.networks.clear();
			tean.Init();
		}
		return 0;
	}

	//###################################################################################################################################

	int Draw(float dTime) {

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

		for (Bird* bird : birds) {
			DrawRotatedDecal({ round(bird->x + (bird_w / 2)), round(bird->y + (bird_h / 2)) }, bird->Animate(dTime), bird->angle, { bird_w / (2.0f * scale), bird_h / (2.0f * scale) }, { scale,scale });
		}
		DrawDecal({ g_x, 675 }, ground_image, { scale,scale });
		DrawDecal({ g_x + screen_w, 675 }, ground_image, { scale,scale });

		DrawStringDecal({ TextCenter() - 3, 103 }, std::to_string(score), olc::BLACK, { 3.01f * scale, 4.1f * scale });
		DrawStringDecal({ TextCenter(), 100 }, std::to_string(score), olc::WHITE, { 3 * scale, 4 * scale });

		//DrawCircle({ (int)round(pipeList[0].x) , (int)round(pipeList[0].y) }, 1, olc::RED);
		//DrawCircle({ (int)round(pipeList[0].x) , (int)round(pipeList[0].y-161) }, 1, olc::RED);

		return 0;
	}

	int DrawNetwork() {

			FillRectDecal({ screen_w, 0 }, { true_screen_w - screen_w, true_screen_h }, olc::BLACK);
		
		return 0;
	}

	float TextCenter() {
		int digit = 0;

		for (char i : std::to_string(score)) {
			digit++;
		}
		return ((screen_w - digit * (8 * 3 * scale)) / 2.f);
	}

	//###################################################################################################################################

	bool OnUserUpdate(float dTime) override {
		GameLoop(dTime);
		Draw(dTime);
		DrawNetwork();

		//std::thread t1(&Engine::GameLoop, this, dTime);
		//std::thread t2(&Engine::Draw, this, dTime);

		//t1.join();
		//t2.join();
		return true;
	}

};


int main()
{
	int pixelSize = 1;

	/*
	while (true)
	{
		for (double result : head.netowk->Activate({ 1., 2., 3. })) {
			std::cout << result << "\n";
		}
		head = neat::NEAT();
	}
	*/

	//neat::NEAT* z = new neat::NEAT();

	Engine engine; {

		if (engine.Construct(true_screen_w, true_screen_h, pixelSize, pixelSize)) {
			engine.Start();
		}
		return 0;
	}
	//neat::DrawTheNetwork();
}



