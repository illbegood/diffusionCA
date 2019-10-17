#ifndef MODEL_H
#define MODEL_H

#include "controller.h"
#include <vector>
#include <string>

using namespace std;

class Controller;
class CA2D;

class CA2D {
public:
	struct Cell2D {
		//max starting value, may grow bigger afterwards
		const unsigned int maxParticles = (unsigned)-1;
		unsigned particles = 0;
		Cell2D(){ }
		Cell2D(unsigned i) {
			if (i <= maxParticles)
				particles = i;
		}
		Cell2D &operator =(const Cell2D &);
	};

	CA2D() {}

	CA2D(size_t x, size_t y, double p, vector<unsigned> values);

	static int cmd_step(vector<string>& args, Controller &c);

	static int cmd_delta(vector<string>& args, Controller &c);

	size_t getSizeX();

	size_t getSizeY();

	size_t getSize();

	double getPMove();

	vector<unsigned> getParticles();

private:
	const size_t minSize = 1, maxSize = 1000,
		minSizeX = minSize, minSizeY = minSize,
		maxSizeX = maxSize, maxSizeY = maxSize;
	size_t sizeX, sizeY, size;

	const unsigned dimension2D = 2;
	const unsigned directions2D = dimension2D * 2;
	const static unsigned maxStep = -1;

	double pMove;

	vector<Cell2D> *grid, *newGrid;

	Cell2D &get(vector<Cell2D> *grid, size_t x, size_t y);

	void set(vector<Cell2D> *grid, size_t x, size_t y, unsigned value);

	void step(unsigned count);

	unsigned getMovingParticles(Cell2D &cell);

	void checkOOBAndAddParticles(vector<Cell2D> *grid, long long x, long long y, unsigned amount);

	double random01();

	unsigned randomUInt(unsigned max);

	static bool isInClosedInterval(long long x, long long min, long long max);

	bool isOutOfBounds(long long x, long long y);

};
#endif
