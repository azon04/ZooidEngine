#include "SDFGenerator.h"

#include <stdlib.h>


#if !defined(SDF_ALLOC)
#define SDF_ALLOC(size) malloc(size)
#endif

#if !defined(SDF_FREE)
#define SDF_FREE(ptr) free(ptr)
#endif

#if !defined(SDF_SQRT)
#include <math.h>
#define SDF_SQRT(v) sqrt(v)
#endif

struct Point
{
	int dx;
	int dy;
};

const Point inside{ 0,0 };
const Point outside{ 999, 999 };

struct Grid
{
	int mWidth;
	int mHeight;
	Point* mPoints;
};

int squareDist(const Point& p) { return p.dx * p.dx + p.dy * p.dy; }
float distance(const Point& p) { return SDF_SQRT(squareDist(p)); }
void getPoint(Grid& grid, int x, int y, Point& p)
{
	if (x < 0 || x >= grid.mWidth || y < 0 || y >= grid.mHeight)
		p = Point{ 0,0 };
	else
		p = grid.mPoints[x + y * grid.mWidth];
}

void comp(Grid& grid, Point& p, int x, int y, int offsetX, int offsetY)
{
	Point other;
	getPoint(grid, x + offsetX, y + offsetY, other);
	other.dx += offsetX;
	other.dy += offsetY;

	if (squareDist(p) > squareDist(other))
		p = other;
}

void store(Grid& grid, const Point& p, int x, int y)
{
	grid.mPoints[x + y * grid.mWidth] = p;
}

void generateSDF(Grid& grid)
{
	// Pass 1
	for (int y = 0; y < grid.mHeight; y++)
	{
		// Mask 1
		for (int x = 0; x < grid.mWidth; x++)
		{
			Point p;
			getPoint(grid, x, y, p);
			comp(grid, p, x, y, -1, 0);
			comp(grid, p, x, y, 0, -1);
#if SDF_METHOD == _8SSEDT
			comp(grid, p, x, y, -1, -1);
			comp(grid, p, x, y, 1, -1);
#endif
			store(grid, p, x, y);
		}

		// Mask 2
		for (int x = grid.mWidth - 1; x >= 0; x--)
		{
			Point p;
			getPoint(grid, x, y, p);
			comp(grid, p, x, y, 1, 0);
			store(grid, p, x, y);
		}
	}

	// Pass 2
	for (int y = 0; y < grid.mHeight; y++)
	{
		// Mask 4
		for (int x = grid.mWidth - 1; x >= 0; x--)
		{
			Point p;
			getPoint(grid, x, y, p);
			comp(grid, p, x, y, 1, 0);
			comp(grid, p, x, y, 0, 1);
#if SDF_METHOD == _8SSEDT
			comp(grid, p, x, y, -1, 1);
			comp(grid, p, x, y, 1, 1);
#endif
			store(grid, p, x, y);
		}

		// Mask 3
		for (int x = 0; x < grid.mWidth; x++)
		{
			Point p;
			getPoint(grid, x, y, p);
			comp(grid, p, x, y, -1, 0);
			store(grid, p, x, y);
		}
	}
}

sdf_uchar getBufferAt(sdf_uchar* buffer, int x, int y, sdf_uint width, sdf_uint height)
{
	return buffer[x + y * width];
}

void storeBufferAt(sdf_uchar* buffer, sdf_uchar c, int x, int y, sdf_uint width, sdf_uint height)
{
	buffer[x + y * width] = c;
}

void generate_sdf_from_image(sdf_uchar* imageBuffer, sdf_uint width, sdf_uint height, sdf_uchar* ouputBuffer)
{
	Grid grid1{ width, height, (Point *)SDF_ALLOC(sizeof(Point) * width * height) };
	Grid grid2{ width, height, (Point *)SDF_ALLOC(sizeof(Point) * width * height) };

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			sdf_uchar c = getBufferAt(imageBuffer, x, y, width, height);
			
			if (c < 128)
			{
				store(grid1, inside, x, y);
				store(grid2, outside, x, y);
			}
			else
			{
				store(grid2, inside, x, y);
				store(grid1, outside, x, y);
			}
		}
	}

	generateSDF(grid1);
	generateSDF(grid2);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Point p1, p2;
			getPoint(grid1, x, y, p1);
			getPoint(grid2, x, y, p2);

			float dist1 = distance(p1);
			float dist2 = distance(p2);
			float dist = (dist1 - dist2);

			// Clamp and scale
			int c = (128 + dist * 4);
			if (c < 0) c = 0;
			if (c > 255) c = 255;

			storeBufferAt(ouputBuffer, c, x, y, width, height);
		}
	}

	SDF_FREE(grid1.mPoints);
	SDF_FREE(grid2.mPoints);
}
