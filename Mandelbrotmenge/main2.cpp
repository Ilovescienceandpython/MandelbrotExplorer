/*#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <string>

#include "Mandelbrot.h"

int XSIZE = 1920;
int YSIZE = 1080;
int COUNT = 1000;
double MOVEPOS = 2.8;
double STEP = 0.2;
int STARTPOS = 0;

void Save(sf::Image image, int i)
{
	image.saveToFile("images/test_" + std::to_string(i) + "_.png");
}

int main(int argn, char *argv[])
{
	std::thread *th = NULL;
	if (argn > 6)
	{
		XSIZE = std::stoi(argv[1]);
		YSIZE = std::stoi(argv[2]);
		COUNT = std::stoi(argv[3]);
		MOVEPOS = std::stof(argv[4]);
		STEP = std::stof(argv[5]);
		STARTPOS = std::stoi(argv[6]);
	}
	Mandelbrot mb(XSIZE, YSIZE);
	sf::RenderTexture rt;
	rt.create(XSIZE, YSIZE);

	mb.Move(sf::Vector2i(XSIZE / MOVEPOS, 0), false);
	mb.ResetMove();

	for (size_t i = 0; i < STARTPOS; i++)
	{
		mb.Zoom(sf::Vector2i(XSIZE / 2, YSIZE / 2), STEP, false);
	}

	for (size_t i = 0; i < COUNT; i++)
	{
		mb.Zoom(sf::Vector2i(XSIZE / 2, YSIZE / 2), STEP);
		rt.draw(mb);
		//rt.getTexture().copyToImage().saveToFile();
		if (th != NULL)
		{
			th->join();
		}
		delete th;
		th = new std::thread(Save, rt.getTexture().copyToImage(), i + STARTPOS);
	}

	return 0;

}
*/