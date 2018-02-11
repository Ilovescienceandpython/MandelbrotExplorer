#include "Mandelbrot.h"
#include <iostream>
#include <thread>
using namespace std;
Mandelbrot::Mandelbrot(int x, int y) : m_Dscreen(0, x, 0, y), m_Dfract(-1.5, 1.5, -1.5, 1.5), m_Iiter_max(50), m_Ix(x), m_Iy(y), m_Ddx(0), m_Ddy(0)
{
	m_Ssprite.setTexture(m_Ttexture);

	//if ((double)x / (double)y != 1.0)
	{
		SetSize(sf::Vector2u(x, y), false);
	}

}


Mandelbrot::~Mandelbrot()
{
}

inline double mmap(double value, double start1, double stop1, double start2, double stop2)
{
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}
/*
//Use slices so the main programm must wait until the last thread is finish
inline void Mandelbrot::Tile(sf::Uint8 *pixels, double* XPosition, double* YPosition, unsigned char * C, int starty, int endy)
{
	sf::Clock ztime;
	double zx, zy, zx2, zy2;
	unsigned int n = 0;
	for (size_t i = m_Dscreen.y_min() + starty; i < endy; i++)
	{
		for (size_t j = m_Dscreen.x_min(); j < m_Dscreen.x_max(); j++)
		{
			zx = zy = zx2 = zy2 = 0;
			n = 0;


			for (; n < m_Iiter_max && zx2 + zy2 < 4; n++) {
				zy = 2 * zx * zy + YPosition[i];
				zx = zx2 - zy2 + XPosition[j];
				zx2 = zx * zx;
				zy2 = zy * zy;
			}

			unsigned int index = ((m_Ix * i) + j) * 4;

			pixels[index] = C[n*3];
			pixels[index + 1] = C[n*3 + 1];
			pixels[index + 2] = C[n*3 + 2];
			pixels[index + 3] = 255;
		}
	}
	mutex.lock();
	std::cout << this_thread::get_id() << ": " << ztime.restart().asMicroseconds() << std::endl;
	mutex.unlock();
}
*/

inline void Mandelbrot::Tile(sf::Uint8 *pixels, double* XPosition, double* YPosition,unsigned int *Index, unsigned char * C, int starty, int step, int endy)
{
	double zx, zy, zx2, zy2;

	unsigned int n = 0;
	for (size_t i = starty; i < endy; i+=step)
	{
		zx = zy = zx2 = zy2 = 0;
		n = 0;
		int y = i % m_Ix; 
		int x = (i - y) / m_Ix;

		//int y = Index[i * 2];
		//int x = Index[i * 2 + 1];

		for (; n < m_Iiter_max && zx2 + zy2 < 4; n++) {
			zy = 2 * zx * zy + YPosition[x];
			zx = zx2 - zy2 + XPosition[y];
			zx2 = zx * zx;
			zy2 = zy * zy;
		}

		//unsigned int index = ((m_Ix * i) + j) * 4;
	
		pixels[i*4] = C[n * 3];
		pixels[i*4 + 1] = C[n * 3 + 1];
		pixels[i*4 + 2] = C[n * 3 + 2];
		pixels[i*4 + 3] = 255;
	}
}

inline void Mandelbrot::Update()
{
	std::vector<std::thread*> threads;
	int NumberOfThreads = std::thread::hardware_concurrency();

	for (int y = 0; y < NumberOfThreads; y++)
	{
		threads.push_back(new std::thread(&Mandelbrot::Tile, this, pixels, PositionsX, PositionsY,Indexes, Colors, y, NumberOfThreads, m_Ix * m_Iy));
	}

	for (size_t i = 0; i < NumberOfThreads; i++)
	{
		threads.at(i)->join();
		delete threads[i];
		threads[i] = NULL;
	}

	m_Iimage.create(m_Ix, m_Iy, pixels);
	m_Ttexture.loadFromImage(m_Iimage);
	m_Ssprite.setTextureRect(sf::IntRect(0, 0, m_Ttexture.getSize().x, m_Ttexture.getSize().y));
	m_Ssprite.setTexture(m_Ttexture);
}
/*
//Slice same as above
inline void Mandelbrot::Update()
{
	std::cout << "--------------" << std::endl;
	std::vector<std::thread*> threads;
	int NumberOfThreads = std::thread::hardware_concurrency();

	for (int y = 0; y < NumberOfThreads; y++)
	{
		int ytiles = m_Dscreen.height() / NumberOfThreads;
		int starty = y * ytiles;
		int endy = (y + 1) * ytiles;
		threads.push_back(new std::thread(&Mandelbrot::Tile, this, pixels, PositionsX, PositionsY, Colors, starty, endy));
	}

	for (size_t i = 0; i < NumberOfThreads; i++)
	{
		threads.at(i)->join();
		delete threads[i];
		threads[i] = NULL;
	}

	m_Iimage.create(m_Ix, m_Iy, pixels);
	m_Ttexture.loadFromImage(m_Iimage);
	m_Ssprite.setTextureRect(sf::IntRect(0, 0, m_Ttexture.getSize().x, m_Ttexture.getSize().y));
	m_Ssprite.setTexture(m_Ttexture);
}
*/
void Mandelbrot::Zoom(sf::Vector2i mousexy, double delta, bool update)
{
	double px = ((double)mousexy.x / m_Ssprite.getScale().x / (double)m_Dscreen.width() * m_Dfract.width()) + m_Dfract.x_min();
	double py = ((double)mousexy.y / m_Ssprite.getScale().y / (double)m_Dscreen.height() * m_Dfract.height()) + m_Dfract.y_min();


	double t = 0.1 * delta;

	double xmax = abs(m_Dfract.x_max() - px);
	double xmin = abs(m_Dfract.x_min() - px);
	double ymax = abs(m_Dfract.y_max() - py);
	double ymin = abs(m_Dfract.y_min() - py);

	m_Dfract.reset(m_Dfract.x_min() + t * xmin, m_Dfract.x_max() - t * xmax, m_Dfract.y_min() + t * ymin, m_Dfract.y_max() - t * ymax);
	for (size_t i = 0; i < m_Ix; i++)
	{
		PositionsX[i] = (double)i / m_Dscreen.width() * m_Dfract.width() + m_Dfract.x_min();
	}
	for (size_t i = 0; i < m_Iy; i++)
	{
		PositionsY[i] = (double)i / m_Dscreen.height() * m_Dfract.height() + m_Dfract.y_min();
	}


	if (update)
	{
		Update();
	}
}

void Mandelbrot::Scale(sf::Vector2f s)
{
	m_Ssprite.setScale(s);
}

void Mandelbrot::Move(sf::Vector2i v, bool update)
{
	double dx = (mmap(v.x, m_Dscreen.x_min(), m_Dscreen.x_max(), m_Dfract.x_min(), m_Dfract.x_max()) - m_Dfract.x_min()) / m_Ssprite.getScale().x;
	double dy = (mmap(v.y, m_Dscreen.y_min(), m_Dscreen.y_max(), m_Dfract.y_min(), m_Dfract.y_max()) - m_Dfract.y_min()) / m_Ssprite.getScale().y;

	m_Dfract.reset(m_Dfract.x_min() + m_Ddx - dx, m_Dfract.x_max() + m_Ddx - dx, m_Dfract.y_min() + m_Ddy - dy, m_Dfract.y_max() + m_Ddy - dy);

	m_Ddx = dx;
	m_Ddy = dy;

	for (size_t i = 0; i < m_Ix; i++)
	{
		PositionsX[i] = (double)i / m_Dscreen.width() * m_Dfract.width() + m_Dfract.x_min();
	}
	for (size_t i = 0; i < m_Iy; i++)
	{
		PositionsY[i] = (double)i / m_Dscreen.height() * m_Dfract.height() + m_Dfract.y_min();
	}

	if (update)
	{
		Update();
	}
}

void Mandelbrot::ResetMove()
{
	m_Ddx = 0;
	m_Ddy = 0;
}

void Mandelbrot::SetMaxIter(int v)
{
	m_Iiter_max = v;
	if (Colors != NULL)
	{
		delete Colors;
		Colors = NULL;
	}

	Colors = new unsigned char[(m_Iiter_max + 1) * 3];

	for (int i = 0; i <= m_Iiter_max; i++)
	{
		unsigned char * c = get_rgb_piecewise_linear(i);
		Colors[i * 3] = c[0];
		Colors[i * 3 + 1] = c[1];
		Colors[i * 3 + 2] = c[2];

	}
}

void Mandelbrot::SetSize(sf::Vector2u size, bool update)
{
	if (pixels != NULL)
	{
		delete pixels;
		pixels = NULL;
	}
	m_Dscreen.reset(0, size.x, 0, size.y);
	m_Ix = size.x;
	m_Iy = size.y;
	double ratio = (double)m_Ix / (double)m_Iy;
	m_Dfract.reset(m_Dfract.x_min()*ratio, m_Dfract.x_max() * ratio, m_Dfract.x_min(), m_Dfract.x_max());
	pixels = new sf::Uint8[(m_Ix * m_Iy) * 4];
	if (PositionsX != NULL)
	{
		delete PositionsX;
		PositionsX = NULL;
	}
	PositionsX = new double[m_Ix];

	for (size_t i = 0; i < m_Ix; i++)
	{
		PositionsX[i] = (double)i / m_Dscreen.width() * m_Dfract.width() + m_Dfract.x_min();
	}

	if (PositionsY != NULL)
	{
		delete PositionsY;
		PositionsY = NULL;
	}
	PositionsY = new double[m_Iy];
	for (size_t i = 0; i < m_Iy; i++)
	{
		PositionsY[i] = (double)i / m_Dscreen.height() * m_Dfract.height() + m_Dfract.y_min();
	}

	if (Colors != NULL)
	{
		delete Colors;
		Colors = NULL;
	}

	Colors = new unsigned char[(m_Iiter_max + 1) * 3];


	for (int i = 0; i <= m_Iiter_max; i++)
	{
		unsigned char * c = get_rgb_piecewise_linear(i);
		Colors[i * 3] = c[0];
		Colors[i * 3 + 1] = c[1];
		Colors[i * 3 + 2] = c[2];

	}

	if (Indexes != NULL)
	{
		delete Indexes;
		Indexes = NULL;
	}

	Indexes = new unsigned int[m_Ix * m_Iy * 2];

	for (size_t i = 0; i < m_Ix * m_Iy; i++)
	{
		Indexes[i*2] = i % m_Ix;
		Indexes[i * 2 + 1] = (i - Indexes[i * 2]) / m_Ix;
	}

	if (update)
	{
		Update();
	}
}


inline void Mandelbrot::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_Ssprite);
}




inline unsigned char * Mandelbrot::get_rgb_piecewise_linear(unsigned int n) {
	int N = 256; // colors per element
	int N2 = N * N;
	int N3 = N * N * N;
	// map n on the 0..1 interval (real numbers)
	double t = (double)n / (double)m_Iiter_max;
	// expand n on the 0 .. 256^3 interval (integers)
	n = (int)(t * (double)N3);

	unsigned char b = n / (N2);
	unsigned int nn = n - b * N2;
	unsigned char r = nn / N;
	unsigned char g = nn - r * N;
	unsigned char ret[3] = { r,g,b };
	return ret;
}

inline unsigned char * Mandelbrot::get_rgb_smooth(unsigned int n) {
	// map n on the 0..1 interval
	double t = (double)n / (double)m_Iiter_max;

	// Use smooth polynomials for r, g, b
	unsigned int r = (int)(9 * (1 - t)*t*t*t * 255);
	unsigned int g = (int)(15 * (1 - t)*(1 - t)*t*t * 255);
	unsigned int b = (int)(8.5*(1 - t)*(1 - t)*(1 - t)*t * 255);
	unsigned char ret[3] = { r,g,b };
	return  ret;
}