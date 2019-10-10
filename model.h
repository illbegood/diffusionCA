#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "controller.h"

using namespace std;

class CA2D {
public:
	struct Cell2D {
		//max starting value, may grow biger afterwards
		const unsigned int maxParticles = (unsigned int)-1;
		unsigned int particles = 0;
		Cell2D(unsigned int i) {
			if (i <= maxParticles)
				particles = i;
		}
	};

	CA2D() {}

	CA2D(size_t x, size_t y, double p, vector<unsigned int> values);

	static int step(vector<string>& args, Controller &c);

	unsigned getSizeX();

	unsigned getSizeY();

	unsigned getPMove();

	vector<unsigned> getParticles();

private:
	const size_t minSize = 1, maxSize = 1000,
		minSizeX = minSize, minSizeY = minSize,
		maxSizeX = maxSize, maxSizeY = maxSize;
	size_t sizeX, sizeY;

	const unsigned directions2D = 4;
	const static unsigned maxStep = -1;

	double pMove;

	vector<Cell2D&> grid;

	Cell2D& get(vector<Cell2D&> &grid, size_t x, size_t y);

	void init(vector<Cell2D&> &grid);

	void stepN(unsigned count);

	unsigned getMovingParticles(Cell2D &cell);

	void checkOOBAndAddParticles(vector<Cell2D&> &grid, long long x, long long y, unsigned amount);

	double random01();

	unsigned randomUInt(unsigned max);

	static bool isInClosedInterval(long long x, long long min, long long max);

	bool isOutOfBounds(long long x, long long y);

};
#endif
