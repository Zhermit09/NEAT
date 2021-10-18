#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

olc::Sprite* image;

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
	int x = 0;
	int y = 50;
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		image = new olc::Sprite("");
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}