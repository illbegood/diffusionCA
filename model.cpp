#include "model.h"
#include "controller.h"
#include <cstdlib>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

CA2D::Cell2D & CA2D::Cell2D::operator=(const Cell2D &cell)
{
	particles = cell.particles;
	return *this;
}

CA2D::Cell2D &CA2D::get(vector<Cell2D>* grid, size_t x, size_t y)
{
	return (*grid)[sizeX * y + x];
}

void CA2D::set(vector<Cell2D>* grid, size_t x, size_t y, unsigned value)
{
	Cell2D& cell = (*grid)[sizeX * y + x];
	//assuming value is correct
	cell.particles = value;
}

void CA2D::step(unsigned count) {
	for (unsigned I = 0; I < count; ++I) {
		fill((*newGrid).begin(), (*newGrid).end(), Cell2D(0));
		for (size_t x = 0; x < sizeX; ++x)
			for (size_t y = 0; y < sizeY; ++y) {
				Cell2D &oldCell = get(grid, x, y), &newCell = get(newGrid, x, y);
				unsigned movingParticles = getMovingParticles(oldCell);
				newCell.particles += oldCell.particles - movingParticles;
				unsigned eachDirectionMP = movingParticles / directions2D;
				for (unsigned coordIndex = 0; coordIndex < dimension2D; ++coordIndex)
					for (int delta = -1; delta <= 1; delta += 2) {
						long long nx = coordIndex == 0 ? x + delta : x,
							ny = coordIndex == 1 ? y + delta : y;
						checkOOBAndAddParticles(newGrid, nx, ny, eachDirectionMP);
					}
				unsigned restMP = movingParticles % directions2D;
				for (unsigned i = 0; i < restMP; ++i) {
					unsigned dir = randomUInt(4);
					long long nx = 0, ny = dir & 2 - 1;
					if (dir & 1)
						swap(nx, ny);
					nx += x;
					ny += y;
					checkOOBAndAddParticles(newGrid, nx, ny, 1);
				}
			}
		swap(grid, newGrid);
	}
}

unsigned CA2D::getMovingParticles(Cell2D &cell) {
	double doubleMovingParticles = cell.particles * pMove;
	unsigned truncDMP = (unsigned)trunc(doubleMovingParticles);
	unsigned round = random01() < (doubleMovingParticles - truncDMP) ? 1 : 0;
	return truncDMP + round;
}

void CA2D::checkOOBAndAddParticles(std::vector<Cell2D> *grid, long long x, long long y, unsigned amount) {
	if (isOutOfBounds(x, y))
		return;
	Cell2D &cell = get(grid, x, y);
	cell.particles += amount;
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
	return !(isInClosedInterval(x, 0, sizeX - 1) && isInClosedInterval(y, 0, sizeY - 1));
}

CA2D::CA2D(size_t x, size_t y, double p, vector<unsigned> values) {
	bool ok = false;
	if (pMove >= 0 && pMove < 1) {
		pMove = p;
		//may adjust isOutOfBounds(...) to simplify the condition
		if (isInClosedInterval(x, minSizeX, maxSizeX) && isInClosedInterval(y, minSizeY, maxSizeY)) {
			size = x * y;
			sizeX = x;
			sizeY = y;
			grid = new vector<Cell2D>(size);
			newGrid = new vector<Cell2D>(size);
			for (size_t i = 0; i < size; ++i)
				(*grid)[i].particles = values[i];
			ok = true;
		}
	}
	//if (!ok) throw InvalidArgumentException
}

int CA2D::cmd_step(vector<string>& args, Controller& c)
{
	CA2D &ca = c.getCA();
	//should also catch invalid argument atleast
	int n = (args.size() < 2) ? 1 : stoi(args[1]);
	if (isInClosedInterval(n, 0, maxStep))
		ca.step(n);
	return OK;
}

int CA2D::cmd_delta(vector<string>& args, Controller & c)
{
	//should also catch invalid argument atleast
	size_t x = stoi(args[1]), y = stoi(args[2]);
	double p = args.size() < 4 ? 0.7 : stod(args[3]);
	vector<unsigned> values(x*y);
	values[x * (y / 2) + x / 2] = -1;
	c.setCA(new CA2D(x, y, p, values));
	return OK;
}

size_t CA2D::getSizeX()
{
	return sizeX;
}

size_t CA2D::getSizeY()
{
	return sizeY;
}

size_t CA2D::getSize()
{
	return size;
}

double CA2D::getPMove()
{
	return pMove;
}

vector<unsigned> CA2D::getParticles()
{
	vector<unsigned> ret;
	for each (Cell2D cell in *grid)
		ret.push_back(cell.particles);
	return ret;
}
