#include "model.h"
#include "controller.h"
#include <cstdlib>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

CA2D::Cell2D& CA2D::get(vector<Cell2D&>& grid, size_t x, size_t y)
{
	return grid[sizeX * y + x];
}

//must be allocated
	void CA2D::init(vector<Cell2D&> &grid) {
		fill(grid.begin(), grid.end(), Cell2D{ 0 });
	}

	void CA2D::stepN(unsigned count) {
		for (unsigned I = 0; I < count; ++I) {
			vector<Cell2D&> newGrid(grid.size());
			init(newGrid);
			for (size_t x = 0; x < sizeX; ++x)
				for (size_t y = 0; y < sizeX; ++y) {
					Cell2D &cell = get(grid, x, y);
					unsigned movingParticles = getMovingParticles(cell);
					unsigned eachDirectionMP = movingParticles / directions2D;
					int dx = 1, dy = 0;
					for (unsigned i = 0; i < directions2D; ++i) {
						long long nx = x + dx, ny = y + dy;
						checkOOBAndAddParticles(newGrid, nx, ny, eachDirectionMP);
						dx = -(ny - y);
						dy = (nx - x);
					}
					unsigned restMP = movingParticles % directions2D;
					for (unsigned i = 0; i < restMP; ++i) {
						unsigned dir = randomUInt(4), nx, ny;
						nx = 0;
						ny = dir & 2 - 1;
						if (dir & 1)
							swap(nx, ny);
						checkOOBAndAddParticles(newGrid, nx, ny, 1);
					}
				}
			//copy assignment?
			grid = newGrid;
		}
	}

	unsigned CA2D::getMovingParticles(Cell2D &cell) {
		unsigned particles = cell.particles;
		double doubleMovingParticles = particles * pMove;
		unsigned truncDMP = trunc(doubleMovingParticles);
		unsigned round = random01() < (doubleMovingParticles - truncDMP) ? 1 : 0;
		return truncDMP + round;
	}

	void CA2D::checkOOBAndAddParticles(std::vector<Cell2D&> &grid, long long x, long long y, unsigned amount) {
		if (isOutOfBounds(x, y))
			return;
		get(grid, x, y).particles += amount;
	}

	double CA2D::random01() {
		uniform_real_distribution<double> unif(0, 1);
		default_random_engine re;
		return unif(re);
	}

	unsigned CA2D::randomUInt(unsigned max) {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(0, max);
		return dis(gen);
	}

	//might as well add template
	bool CA2D::isInClosedInterval(long long x, long long min, long long max) {
		return x >= min && x <= max;
	}

	bool CA2D::isOutOfBounds(long long x, long long y) {
		return !(isInClosedInterval(x, 0, sizeX) && isInClosedInterval(y, 0, sizeY));
	}

	CA2D::CA2D(size_t x, size_t y, double p, vector<unsigned> values) {
		bool ok = false;
		if (pMove >= 0 && pMove < 1) {
			pMove = p;
			if (isInClosedInterval(x, minSizeX, maxSizeX) && isInClosedInterval(y, minSizeY, maxSizeY)) {
				sizeX = x;
				sizeY = y;
				grid.reserve(sizeX * sizeY);
				size_t size = grid.size();
				for (size_t i = 0; i < size; ++i)
					grid[i].particles = values[i];
				ok = true;
			}
		}
		//if (!ok) throw InvalidArgumentException
	}

	int CA2D::step(vector<string>& args, Controller& c)
	{
		void * ret;
		CA2D &ca = c.getCA();
		int n = (args.size() < 2) ? 1 : stoi(args[1]);
		if (isInClosedInterval(n, 0, maxStep))
			ca.stepN(n);
	}

	unsigned CA2D::getSizeX()
	{
		return sizeX;
	}

	unsigned CA2D::getSizeY()
	{
		return sizeY;
	}

	unsigned CA2D::getPMove()
	{
		return pMove;
	}

	vector<unsigned> CA2D::getParticles()
	{
		vector<unsigned> ret;
		for each (Cell2D cell in grid)
			ret.push_back(cell.particles);
		return ret;
	}
