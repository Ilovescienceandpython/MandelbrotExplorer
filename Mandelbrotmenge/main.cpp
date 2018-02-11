#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <string> 

#include "Mandelbrot.h"

const int XSIZE = 500;
const int YSIZE = 500;

Mandelbrot mb(XSIZE, YSIZE);

void Console()
{
	std::string input = "";
	while (input != "E")
	{
		std::cin >> input;
		mb.SetMaxIter(atoi(input.c_str()));
		mb.Update();
	}
}

int main()
{

	sf::RenderWindow window(sf::VideoMode(XSIZE, YSIZE), "SFML works!");
	std::thread t(Console);

	//sf::RenderTexture rt;
	//rt.create(XSIZE, YSIZE);
	//sf::Sprite sprite(rt.getTexture());
	//sprite.setPosition(0, 0);
	//mb.Scale(sf::Vector2f(5,5));
	window.setVerticalSyncEnabled(true);
	sf::Clock MoveClock;
	mb.Update();
	sf::Vector2i startpos;
	bool Click = false;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::MouseWheelScrolled)
			{

				sf::Vector2i mp = sf::Mouse::getPosition(window);

				if (mp.x >= 0 && mp.y >= 0)
				{
					mb.Zoom(mp, event.mouseWheelScroll.delta);
				}
			}
			else if (event.type == sf::Event::Resized)
			{
				window.setView(sf::View(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y)));
				mb.SetSize(window.getSize());
				//rt.create(window.getSize().x, window.getSize().y);
			}
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (!Click)
			{
				startpos = sf::Mouse::getPosition(window);
			}

			sf::Vector2i mp = sf::Mouse::getPosition(window);
			if (mp.x >= 0 && mp.y >= 0 && mp.x <= window.getSize().x && mp.y <= window.getSize().y)
			{
				mb.Move(mp - startpos);
			}
			Click = true;
		}
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && Click)
		{
			mb.ResetMove();
			Click = false;
		}

		window.clear();
		//rt.draw(mb);
		window.draw(mb);
		window.display();
	}
	t.detach();
	return 0;

	/*
		sf::RenderTexture rt;
		rt.create(XSIZE, YSIZE);

		mb.Move(sf::Vector2i(600, 0),false);
		mb.ResetMove();

		for (size_t i = 0; i < 100; i++)
		{
			mb.Zoom(sf::Vector2i(XSIZE/2, YSIZE / 2),1);
			rt.draw(mb);
			rt.getTexture().copyToImage().saveToFile("images/test_" + std::to_string(i) + "_.png");
		}


		return 0;
	*/
}
