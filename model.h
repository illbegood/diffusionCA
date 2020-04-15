#ifndef MODEL_H
#define MODEL_H

#include "controller.h"
#include <map>
#include <vector>
#include <string>
#include <random>

using namespace std;

enum Mode { FOR_EACH_PARTICLE = 0, EACH_DIRECTION_EVENLY = 1 };
enum Border { NONE = 0, IMPENETRABLE = 1 };


class CA {
public:
	const static unsigned initialCount = 1e7;
	int mpi_rank = 0;
	struct Cell {
		//max starting value, may grow bigger afterwards
		const unsigned int maxParticles = (unsigned)-1;
		unsigned particles = 0;
		Cell() { }
		Cell(unsigned i) {
			if (i <= maxParticles)
				particles = i;
		}
		Cell &operator =(const Cell &);
	};

	CA() {}

	~CA();

	CA(const vector<size_t> &size, double p, vector<unsigned> values);

	/*CA(vector<size_t> &size, double p);*/

	static int cmd_step(const vector<string>& args, Controller &c);

	static int cmd_delta(const vector<string>& args, Controller &c);

	static int cmd_blank(const vector<string>& args, Controller &c);

	static int cmd_mode(const vector<string>& args, Controller &c);

	static int cmd_border(const vector<string>& args, Controller &c);

	void set(const vector<size_t> &coord, unsigned value);
	
	vector<size_t> getSize();

	size_t getTotalSize();

	double getPMove();

	vector<unsigned> getParticles();

	void stepBounds();

	void stepNoBounds();

	vector<unsigned> getLeftBorder();

	vector<unsigned> getRightBorder();

	void addToLeftBorder(const vector<unsigned>& values);

	void addToRightBorder(const vector<unsigned>& values);

	void finishStep();

	bool testN(vector<size_t> &v) {
		Cell &c = get(newGrid, v);
		return c.particles != 0;
	}

	bool testO(vector<size_t> &v) {
		Cell &c = get(grid, v);
		return c.particles != 0;
	}

	static unsigned inc(vector<size_t> &coord, const vector<size_t> &size);

	void save(string msg, string name);

	static size_t getIndex(const vector<size_t> &coord, vector<size_t> &size);

private:
	int debug = 0;
	const static unsigned maxDimensions = 3;
	unsigned dimensions;
	const static size_t min_Size = 1, max_Size = 1000;
	const size_t minSize[maxDimensions] = { min_Size, min_Size, min_Size },
		maxSize[maxDimensions] = { max_Size, max_Size, max_Size };
	size_t totalSize, crossSectionSize;
	vector<size_t> size;

	const static unsigned maxStep = -1;

	double pMove;

	vector<Cell> *grid, *newGrid;

	uniform_real_distribution<double> urd;
	uniform_int_distribution<> uid;
	default_random_engine re;

	Mode mode = EACH_DIRECTION_EVENLY;
	Border border = NONE;

	vector<size_t> coords(size_t index);

	Cell &get(vector<Cell> *grid, size_t i);

	Cell &get(vector<Cell> *grid, vector<size_t> &coord);

	void set(vector<Cell> *grid, vector<size_t> &coord, unsigned value);

	void stepN(unsigned count);

	void step(size_t first, size_t last);

	unsigned getMovingParticles(Cell &cell);

	void moveParticleRandom(Cell &cell, vector<size_t> &coord);

	void moveParticles(Cell &cell, vector<size_t> &coord, unsigned amount, int delta, unsigned dim);

	double random01();

	unsigned randomUInt();

	static bool isInClosedInterval(long long x, long long min, long long max);

	void inc(vector<size_t> &coord);

};

int cmd_green(const vector<string>& args, Controller &c);

#endif
