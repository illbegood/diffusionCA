#define _USE_MATH_DEFINES

#include "model.h"
#include "controller.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include <iostream>

using namespace std;

CA::Cell & CA::Cell::operator=(const Cell &cell)
{
	particles = cell.particles;
	return *this;
}

vector<size_t> CA::coords(size_t index) {
	vector<size_t> ret(dimensions);
	for (unsigned i = 0; i < dimensions; ++i) {
		ret[i] = index % size[i];
		index /= size[i];
	}
	return ret;
}

size_t CA::getIndex(const vector<size_t> &coord, vector<size_t> &size)
{
	size_t index = 0;
	unsigned dim = coord.size();
	for (unsigned i = 0; i < dim; ++i) {
		size_t mul = 1;
		for (unsigned j = 0; j < i; ++j)
			mul *= size[j];
		index += mul * coord[i];
	}
	return index;
}

CA::Cell &CA::get(vector<Cell>* grid, size_t i)
{
	return (*grid)[i];
}

CA::Cell &CA::get(vector<Cell> *grid, vector<size_t> &coord)
{
	return (*grid)[getIndex(coord, size)];
}

void CA::set(vector<Cell> *grid, vector<size_t> &coord, unsigned value)
{
	Cell& cell = (*grid)[getIndex(coord, size)];
	//assuming value is correct
	cell.particles = value;
}

void CA::stepN(unsigned count)
{
	for (unsigned i = 0; i < count; ++i) {
		step(0, totalSize);
		finishStep();
	}
}

unsigned CA::inc(vector<size_t> &coord, const vector<size_t> &size)
{
	unsigned i;
	for (i = 0; i < coord.size(); ++i) {
		if (coord[i]++ < size[i] - 1)
			break;
		coord[i] = 0;
	}
	return i;
}

#include <fstream>

void CA::save(string msg = "", string name = "") {
	ofstream file(name);
	file << msg << endl;
	IO::save(*this, file);
	file.close();
}

void CA::step(size_t first, size_t last) {
	//if (mpi_rank == 0)
		//std::cout << "step begin " << mpi_rank << " " << first << "-" << last << endl;
	//save(to_string(first) + " " + to_string(last), "o_" + to_string(mpi_rank) + "_" + to_string(debug));
	vector<size_t> lastCoord = coords(last);
	auto gridIterator = (*grid).begin(), newGridIterator = (*newGrid).begin();
	int cnt = 0, allCnt = 0;
	size_t pos = first;
	for (vector<size_t> coord = coords(first); pos != last; inc(coord)) {
		//Cell &oldCell = *gridIterator, &newCell = *newGridIterator;
		Cell &oldCell = (*grid)[pos], &newCell = (*newGrid)[pos];
		/*for (auto i = coord.begin(); i != coord.end(); ++i)
			cout << *i << " ";
		cout << oldCell.particles << " ";*/
		//save("", to_string(pos));
		//cout << pos << " " << oldCell.particles << endl;
		if (oldCell.particles != 0) {
			//cout << 'x' << oldCell.particles;
			unsigned movingParticles = getMovingParticles(oldCell);
			newCell.particles += oldCell.particles - movingParticles;
			if (mode == EACH_DIRECTION_EVENLY) {
				unsigned eachDirectionMP = movingParticles / (dimensions * 2);
				for (unsigned coordIndex = 0; coordIndex < dimensions; ++coordIndex)
					for (int delta = -1; delta <= 1; delta += 2)
						moveParticles(newCell, coord, eachDirectionMP, delta, coordIndex);
				unsigned restMP = movingParticles % (dimensions * 2);
				for (unsigned i = 0; i < restMP; ++i)
					moveParticleRandom(newCell, coord);
			}
			else if (mode == FOR_EACH_PARTICLE)
				for (unsigned i = 0; i < movingParticles; ++i)
					moveParticleRandom(newCell, coord);
			//cout << newCell.particles << endl;
			cnt++;
		}
		pos++;
		allCnt++;
		gridIterator++;
		newGridIterator++;
	}
	/*
	std::swap(grid, newGrid);
	save(to_string(first) + " " + to_string(last), "n_" + to_string(mpi_rank) + "_" + to_string(debug++));
	std::swap(grid, newGrid);
	//if (mpi_rank == 0)
		std::cout << "step end " << mpi_rank  << " " << first << "-"  << last << " " << allCnt << " " << cnt << endl;
		*/
}

unsigned CA::getMovingParticles(Cell &cell) {
	double doubleMovingParticles = cell.particles * pMove;
	unsigned truncDMP = (unsigned)trunc(doubleMovingParticles);
	unsigned round = random01() < (doubleMovingParticles - truncDMP) ? 1 : 0;
	return truncDMP + round;
}

void CA::moveParticleRandom(Cell &cell, vector<size_t> &coord) {
	unsigned dir = randomUInt();
	int delta = dir % 2 ? 1 : -1;
	moveParticles(cell, coord, 1, delta, dir >> 1);
}

void CA::moveParticles(Cell &cell, vector<size_t> &coord, unsigned amount, int delta, unsigned dim) {
	bool outOfBounds = false;
	size_t &coordToChange = coord[dim];
	if (!isInClosedInterval(coordToChange + delta, 0, size[dim] - 1)) {
		if (border == IMPENETRABLE)
			cell.particles += amount;
	}
	else {
		coordToChange += delta;
		Cell &cell = get(newGrid, coord);
		cell.particles += amount;
		coordToChange -= delta;
	}
}

double CA::random01() {
	return urd(re);
}

unsigned CA::randomUInt() {
	return uid(re);
}

//might as well add template
bool CA::isInClosedInterval(long long x, long long min, long long max) {
	return x >= min && x <= max;
}

void CA::inc(vector<size_t>& coord)
{
	inc(coord, size);
}

CA::~CA()
{
	delete grid;
	delete newGrid;
}

CA::CA(const vector<size_t> &size, double p, vector<unsigned> values) :uid(0, 2 * (dimensions = size.size()) - 1), urd(0, 1) {
	bool ok = false;
	if (pMove >= 0 && pMove <= 1) {
		pMove = p;
		bool withinBounds = true;
		for (unsigned i = 0; i < dimensions; ++i)
			withinBounds &= isInClosedInterval(size[i], minSize[i], maxSize[i]);
		if (withinBounds) {
			this->size = size;
			totalSize = values.size();
			crossSectionSize = totalSize / size[dimensions - 1];
			grid = new vector<Cell>(totalSize);
			newGrid = new vector<Cell>(totalSize);
			for (size_t i = 0; i < totalSize; ++i)
				(*grid)[i].particles = values[i];
			ok = true;
		}
	}
	//if (!ok) throw InvalidArgumentException
}

int CA::cmd_step(const vector<string>& args, Controller& c)
{
	CA *ca = c.getCA();
	if (!ca)
		return NO_CA_FOUND;
	//should also catch invalid argument atleast
	int n = (args.size() < 2) ? 1 : stoi(args[1]);
	if (isInClosedInterval(n, 0, maxStep))
		ca->stepN(n);
	return OK;
}

int CA::cmd_delta(const vector<string>& args, Controller & c)
{
	//should also catch invalid argument atleast
	unsigned argIndex = 1;
	unsigned dimensions = stoi(args[argIndex++]);
	if (!isInClosedInterval(dimensions, 1, maxDimensions))
		return INVALID_ARGUMENT;
	vector<size_t> size(dimensions);
	size_t totalSize = 1;
	for (unsigned i = 0; i < dimensions; ++i) {
		size[i] = stoi(args[argIndex++]);
		totalSize *= size[i];
	}
	double p = args.size() <= argIndex ? 0.7 : stod(args[argIndex++]);
	vector<unsigned> values(totalSize);
	vector<size_t> center(dimensions);
	for (unsigned i = 0; i < dimensions; ++i)
		center[i] = size[i] / 2;
	values[getIndex(center, size)] = initialCount;//-1;
	CA *ca = new CA(size, p, values);
	c.setCA(ca);
	if (args.size() > argIndex) {
		ca->stepN(stoi(args[argIndex++]));
		if (args.size() > argIndex)
			IO::cmd_save(vector<string>{"save", args[argIndex++]}, c);
	}
	return OK;
}

int CA::cmd_blank(const vector<string>& args, Controller & c)
{
	unsigned argIndex = 1;
	unsigned dimensions = stoi(args[argIndex++]);
	if (!isInClosedInterval(dimensions, 1, maxDimensions))
		return INVALID_ARGUMENT;
	vector<size_t> size(dimensions);
	size_t totalSize = 1;
	for (unsigned i = 0; i < dimensions; ++i) {
		size[i] = stoi(args[argIndex++]);
		totalSize *= size[i];
	}
	double p = args.size() <= argIndex ? 0.7 : stod(args[argIndex++]);
	vector<unsigned> values(totalSize);
	CA *ca = new CA(size, p, values);
	c.setCA(ca);
	return OK;
}

int CA::cmd_mode(const vector<string>& args, Controller & c)
{
	CA *ca = c.getCA();
	if (!ca)
		return NO_CA_FOUND;
	if (args.size() < 2)
		//should be in GUI though
		std::cout << ca->mode << endl;
	else {
		unsigned n = stoi(args[1]);
		if (isInClosedInterval(n, 0, 1))
			ca->mode = (Mode)n;
		else
			return INVALID_ARGUMENT;
	}
	return OK;
}

int CA::cmd_border(const vector<string>& args, Controller & c)
{
	CA *ca = c.getCA();
	if (!ca)
		return NO_CA_FOUND;
	if (args.size() < 2)
		//should be in GUI though
		std::cout << ca->border << endl;
	else {
		unsigned n = stoi(args[1]);
		if (isInClosedInterval(n, 0, 1))
			ca->border = (Border)n;
		else
			return INVALID_ARGUMENT;
	}
	return OK;
}

void CA::set(const vector<size_t>& coord, unsigned value)
{
	Cell& cell = (*grid)[getIndex(coord, size)];
	//assuming value is correct
	cell.particles = value;
}

vector<size_t> CA::getSize()
{
	return vector<size_t>(size);
}

size_t CA::getTotalSize()
{
	return totalSize;
}

double CA::getPMove()
{
	return pMove;
}

vector<unsigned> CA::getParticles()
{
	vector<unsigned> ret;
	for (Cell cell : *grid) {
		ret.push_back(cell.particles);
		//cout << cell.particles << " ";
	}
	//cout << endl;
	return ret;
}

void CA::stepBounds()
{
	step(crossSectionSize, 2 * crossSectionSize);
	step(totalSize - 2 * crossSectionSize, totalSize - crossSectionSize);
}

void CA::stepNoBounds()
{
	step(2 * crossSectionSize, totalSize - 2 * crossSectionSize);
}

vector<unsigned> CA::getLeftBorder()
{
	vector<unsigned> ret;
	for (unsigned i = 0; i < crossSectionSize; ++i)
		ret.push_back((*newGrid)[i].particles);
	return ret;
}

vector<unsigned> CA::getRightBorder()
{
	vector<unsigned> ret;
	for (unsigned i = totalSize - crossSectionSize; i < totalSize; ++i)
		ret.push_back((*newGrid)[i].particles);
	return ret;
}

void CA::addToLeftBorder(const vector<unsigned>& values)
{
	for (int i = 0; i < crossSectionSize; ++i)
		(*newGrid)[crossSectionSize + i].particles += values[i];
}

void CA::addToRightBorder(const vector<unsigned>& values)
{
	for (int i = 0; i < crossSectionSize; ++i)
		(*newGrid)[totalSize - 2 * crossSectionSize + i].particles += values[i];
}

void CA::finishStep()
{
	std::swap(grid, newGrid);
	fill((*newGrid).begin(), (*newGrid).end(), Cell(0));
}


#include <fstream>
//should be somewhere else imo
//may optimize grid from [0..x, 0..y] to [-x/2..x/2, -y/2..y/2] but it's fast enough so far
int cmd_green(const vector<string>& args, Controller & c)
{
	int ret;
	unsigned argIndex = 1;
	if (args.size() <= argIndex)
		return NOT_ENOUGH_ARGUMENTS;
	unsigned dim = stoi(args[argIndex++]);
	if (args.size() <= argIndex + dim)
		return NOT_ENOUGH_ARGUMENTS;
	vector<size_t> size(dim), coord(dim);
	size_t totalSize = 1;
	for (unsigned i = 0; i < dim; ++i) {
		size[i] = stoi(args[argIndex++]);
		totalSize *= size[i];
	}
	if (args.size() <= argIndex + 1)
		return NOT_ENOUGH_ARGUMENTS;
	double k = stod(args[argIndex++]);
	unsigned t = stoi(args[argIndex++]), newLines = 0;
	vector<double> center(dim);
	for (unsigned i = 0; i < dim; ++i)
		center[i] = (size[i] - 1) / 2.0;
	ofstream file(args.size() <= argIndex ? "green" : args[argIndex++]);
	if (file.is_open()) {
		file << "Parameters: " << size.size() << " ";
		for (unsigned i = 0; i < size.size(); ++i)
			file << size[i] << " ";
		file << k << " " << " " << t << endl;
		unsigned count;
		for (count = 0; count < totalSize; ++count) {
			for (unsigned i = 0; i < newLines; ++i)
				if (!(file << endl))
					break;
			double r2 = 0;
			for (unsigned i = 0; i < dim; ++i)
				r2 += (center[i] - coord[i]) * (center[i] - coord[i]);
			double d = CA::initialCount * exp(-r2 / (4 * k*t)) * pow(4 * M_PI*k*t, -0.5 * dim);
			if (!(file << d << " "))
				break;
			newLines = CA::inc(coord, size);
		}
		ret = count == totalSize ? OK : ERROR_WHILE_WRITING;
		file.close();
		return ret;
	}
	return CANNOT_OPEN_FILE;
}