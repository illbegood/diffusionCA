//#define CA_INTERACTIVE
#define CA_MPI
#ifdef CA_INTERACTIVE
#include "controller.h"
#include <iostream>

int main(int argc, char **argv) {
	Controller c;
	c.run(argc, argv);
}
#endif
#ifdef CA_MPI
#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>
#include "io.h"
#include "controller.h"
#include "model.h"
#include <chrono>

using namespace std;

int mpi_rank, mpi_size;
unsigned dim, n, argIndex = 1, lastSize, crossSectionSize, reqCount = 0, mod, quot, thisLastSize, centerIdx;
size_t Left, temp;
vector<unsigned> leftSend, leftRecv, rightSend, rightRecv;
vector<size_t> center;
char ** ARGV;
Controller c;
CA *ca;
vector<string> args;
MPI_Request req[4];


void print(const vector<unsigned>& v) {
	for (auto i = v.begin(); i != v.end(); ++i)
		cout << *i << " ";
	cout << endl;
}

void print(const vector<size_t>& v) {
	for (auto i = v.begin(); i != v.end(); ++i)
		cout << *i << " ";
	cout << endl;
}

void print(const int *a, int size) {
	for (int i = 0; i < size; ++i)
		cout << a[i] << " ";
	cout << endl;
}

void check_center(string s) {
	if (mpi_rank == centerIdx)
		cout << s << endl /*<< ca->testO(center) << ca->testN(center) << endl*/;
}

void createCA() {
	unsigned ret;
	args.push_back("blank");
	//dim
	args.push_back(ARGV[argIndex]);
	dim = stoi(ARGV[argIndex++]);
	//size
	for (unsigned i = 0; i < dim - 1; ++i)
		args.push_back(ARGV[argIndex++]);
	lastSize = stoi(ARGV[argIndex++]);
	mod = lastSize % mpi_size;
	quot = lastSize / mpi_size;
	thisLastSize = quot + (mpi_rank < mod ? 1 : 0);
	Left = 0;
	for (ret = 0; ret < lastSize && Left <= lastSize / 2; ++ret) {
		temp = quot + (ret < mod ? 1 : 0);
		Left += temp;
	}
	thisLastSize += 2;
	args.push_back(to_string(thisLastSize));
	//p
	args.push_back(ARGV[argIndex++]);
	CA::cmd_blank(args, c);
	ca = c.getCA();
	ca->mpi_rank = mpi_rank;
	//if (mpi_rank == 0)
	//cout << ret - 1 << " " << Left << " " << temp << endl;
	centerIdx = --ret;
	if (mpi_rank == ret) {
		vector<size_t> sizes = ca->getSize();
		center.resize(dim);
		for (unsigned i = 0; i < dim - 1; ++i)
			center[i] = sizes[i] / 2;
		center[dim - 1] = lastSize / 2 - (Left - temp) + 1;
		ca->set(center, CA::initialCount);
		//print(center);
	}
	//check_center();
	//cout << "create finished " << mpi_rank << endl;
}

void configureCA() {
	n = stoi(ARGV[argIndex++]);
	char *mask = ARGV[argIndex++];
	args.clear();
	args.push_back("mode");
	args.push_back(mask[0] == '0' ? "0" : "1");
	CA::cmd_mode(args, c);
	args.clear();
	args.push_back("border");
	args.push_back(mask[1] == '0' ? "0" : "1");
	CA::cmd_border(args, c);
	crossSectionSize = ca->getTotalSize() / thisLastSize;
}

void init_pcr() {
	if (mpi_rank != 0) {
		leftSend.resize(crossSectionSize);
		//MPI_Send_init(&leftSend[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + mpi_size - 1) % mpi_size, 123, MPI_COMM_WORLD, &req[reqCount++]);
		leftRecv.resize(crossSectionSize);
		//MPI_Recv_init(&leftRecv[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + mpi_size - 1) % mpi_size, 123, MPI_COMM_WORLD, &req[reqCount++]);
	}
	if (mpi_rank != mpi_size - 1) {
		rightSend.resize(crossSectionSize);
		//MPI_Send_init(&rightSend[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + 1) % mpi_size, 123, MPI_COMM_WORLD, &req[reqCount++]);
		rightRecv.resize(crossSectionSize);
		//MPI_Recv_init(&rightRecv[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + 1) % mpi_size, 123, MPI_COMM_WORLD, &req[reqCount++]);
	}
}

void step() {
	//check_center("Step bounds");
	//cout << "StBo" << mpi_rank << endl;
	ca->stepBounds();
	//cout << "GeBo" << mpi_rank << endl;
	if (mpi_rank != 0) {
		leftSend = ca->getLeftBorder();
		//cout << "leftSend ";
		//print(leftSend);
	}
	if (mpi_rank != mpi_size - 1) {
		rightSend = ca->getRightBorder();
		//cout << "rightSend ";
		//print(rightSend);
	}
	//cout << "BoSe" << mpi_rank << endl;
	//check_center("Startall");
	if (mpi_rank != 0) {
		MPI_Isend(&leftSend[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + mpi_size - 1) % mpi_size, 123, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(&leftRecv[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + mpi_size - 1) % mpi_size, 123, MPI_COMM_WORLD, &req[1]);
	}
	if (mpi_rank != mpi_size - 1) {
		MPI_Isend(&rightSend[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + 1) % mpi_size, 123, MPI_COMM_WORLD, &req[0]);
		MPI_Irecv(&rightRecv[0], crossSectionSize, MPI_UNSIGNED, (mpi_rank + 1) % mpi_size, 123, MPI_COMM_WORLD, &req[1]);
	}
	//MPI_Startall(reqCount, req);
	//check_center("StepNoBounds");
	//cout << "StNB" << mpi_rank << endl;
	ca->stepNoBounds();
	//check_center("Waitall");
	if (mpi_rank != 0 || mpi_rank != mpi_size - 1) {
		MPI_Wait(&req[0], MPI_STATUSES_IGNORE);
		MPI_Wait(&req[1], MPI_STATUSES_IGNORE);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	//MPI_Waitall(reqCount, req, MPI_STATUSES_IGNORE);
	//cout << "BoAd" << mpi_rank << endl;
	if (mpi_rank != 0) {
		ca->addToLeftBorder(leftRecv);
		//cout << "leftRecv ";
		//print(leftRecv);
	}
	if (mpi_rank != mpi_size - 1) {
		ca->addToRightBorder(rightRecv);
		//cout << "rightRecv ";
		//print(rightRecv);
	}
	//cout << "StFi" << mpi_rank << endl;
	//check_center("FinishStep");
	ca->finishStep();
	//check_center("The End");
}

void save() {
	args.clear();
	args.push_back("save");
	args.push_back("Part " + to_string(mpi_rank));
	IO::cmd_save(args, c);
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	//std::cout << "rank " << mpi_rank << endl;
	ARGV = argv;
	createCA();
	//cout << "CrFi" << mpi_rank << endl;
	configureCA();
	//cout << "CoFi" << mpi_rank << endl;
	init_pcr();
	//cout << "IPFi" << mpi_rank << endl;
	auto start = std::chrono::high_resolution_clock::now();
	for (unsigned I = 0; I < n; ++I)
		step();
	auto end = std::chrono::high_resolution_clock::now();
	if (mpi_rank == 0)
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << endl;
	//cout << "ItFi" << mpi_rank << endl;
	//MPI_Barrier(MPI_COMM_WORLD);
	vector<unsigned> values = ca->getParticles();
	vector<unsigned> whole;
	int* recvcounts = 0, *displs = 0;
	/*if (mpi_rank == centerIdx) {
		cout << "rightRecv" << endl;
		print(rightRecv);
		cout << "rightSend" << endl;
		print(rightSend);
	}
	if (mpi_rank == centerIdx + 1) {
		cout << "leftRecv" << endl;
		print(leftRecv);
		cout << "leftSend" << endl;
		print(leftSend);
	}
	MPI_Barrier(MPI_COMM_WORLD);*/
	if (mpi_rank == 0) {
		whole.resize(crossSectionSize * lastSize);
		recvcounts = new int[mpi_size];
		displs = new int[mpi_size];
		size_t offset = 0;
		for (int i = 0; i < mpi_size; ++i) {
			recvcounts[i] = (quot + (i < mod ? 1 : 0)) * crossSectionSize;
			displs[i] = offset;
			offset += recvcounts[i];
		}
		/*
		cout << "recvcounts" << endl;
		print(recvcounts, mpi_size);
		cout << "displs" << endl;
		print(displs, mpi_size);
		*/
	}
	save();
	//cout << "SaFi" << mpi_rank << endl;
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gatherv(&values[crossSectionSize], values.size() - 2 * crossSectionSize, MPI_UNSIGNED, &whole[0], recvcounts, displs, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	/*
	cout << "GaFi" << mpi_rank << endl;
	MPI_Barrier(MPI_COMM_WORLD);
	if (mpi_rank == 0) {
		for (auto val : whole)
			cout << val << " ";
		cout << endl;
	}
	*/
	if (mpi_rank == 0) {
		delete[] recvcounts;
		delete[] displs;
		vector<size_t> dimSize = ca->getSize();
		dimSize[dim - 1] = lastSize;
		c.setCA(new CA(dimSize, ca->getPMove(), whole));
		args.clear();
		args.push_back("save");
		//cout << "arg size " << argc << " argIndex " << argIndex << endl;
		args.push_back(argv[argIndex++]);
		//cout << "SaBe" << endl;
		IO::cmd_save(args, c);
		//cout << "SaEn" << endl;
	}
	MPI_Finalize();
}
#endif