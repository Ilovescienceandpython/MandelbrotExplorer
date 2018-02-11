#pragma once
#include <SFML/Graphics.hpp>
#include "Dimention.h"


class Mandelbrot : public sf::Drawable , public sf::Transformable
{
public:
	Mandelbrot(int x,int y);
	~Mandelbrot();
	inline void Update();
	void Zoom(sf::Vector2i mousexy, double delta,bool update = true);
	void Scale(sf::Vector2f);
	void Move(sf::Vector2i,bool update = true);
	void ResetMove();
	void SetMaxIter(int);
	void SetSize(sf::Vector2u, bool Update = true);
private:
	inline virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
	int m_Iiter_max;
	sf::Image m_Iimage;
	sf::Sprite m_Ssprite;
	sf::Texture m_Ttexture;
	Dimention<int> m_Dscreen;
	Dimention<double> m_Dfract;
	int m_Ix;
	int m_Iy;
	sf::Mutex mutex;
	inline void Tile(sf::Uint8 *pixels, double* XPosition, double* YPosition, unsigned int *Index, unsigned char * C, int starty, int step, int endy);

	double m_Ddx;
	double m_Ddy;
	sf::Uint8* pixels = NULL;
	double * PositionsX = NULL;
	double * PositionsY = NULL;
	unsigned char *Colors = NULL;
	unsigned int *Indexes = NULL;

	inline unsigned char * get_rgb_piecewise_linear(unsigned int n);
	inline unsigned char * get_rgb_smooth(unsigned int n);
};

