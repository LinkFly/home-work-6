#pragma once

#include "Global.h"

#include <limits>
#include <cstdint>
//#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <fstream>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::function;

constexpr auto CIN_MAX = std::numeric_limits<std::streamsize>::max();

//// System helpers
template<typename T>
void error(const T& errMsg) {
	cout << errMsg << endl;
	exit(-1);
}

void pauseExit() {
	cin.get();
}

//// end System helpers

//// Bits handling
template<typename T>
constexpr uint8_t reqBits(const T num) {
	return static_cast<uint8_t>(ceil(log2(num + 1)));
}

uint64_t maxNumByBits(uint8_t nbits) {
	return static_cast<uint64_t>(pow(2, nbits));
}

// (ceiling (log (1- (expt 2 (* 8 8))) 10))
template<class T>
constexpr uint8_t getDigits10() {
	auto maxNum = pow(2.0, std::numeric_limits<T>::digits - 1);
	auto res = static_cast<uint8_t>(ceil(log10(maxNum)));
	return res + std::numeric_limits<T>::is_signed;
}
//// end Bits handling

//// Memory
template<typename T>
void freeMatrix(T** ar, size_t m) {
	for (size_t i = 0; i < m; ++i) {
		delete[] ar[i];
	}
	delete[] ar;
}

// Deleters
struct Deleter {
	void* ptr = nullptr;
	Deleter() = default;
	Deleter(void* ptr) : ptr{ ptr } {}
	~Deleter() {
		delete ptr;
	}
};

struct DeleterArray {
	void* ptr = nullptr;
	DeleterArray(void* ptr) : ptr{ ptr } {}
	~DeleterArray() {
		delete[] ptr;
	}
};


struct DeleterMatrix {
	void** pptr = nullptr;
	size_t rows;

	template<typename T>
	DeleterMatrix(T** pptr, size_t rows) : pptr{ reinterpret_cast<void**>(pptr) }, rows{ rows } {}

	~DeleterMatrix() {
		freeMatrix(pptr, rows);
	}
};

//// Files
void checkOpen(const string& file, const ofstream& ofile) {
	if (!ofile.is_open()) {
		error(string{ "Failed to open file: " } + file);
	}
};

void fillFile(const string& file, const function<void(ostream&)>& fillCallback) {
	ofstream ofile(file);
	checkOpen(file, ofile);
	fillCallback(ofile);
	ofile.close();
}

ifstream::pos_type fileSize(const string& filename) {
	ifstream in(filename, ifstream::ate, ifstream::binary);
	if (!in.is_open()) {
		error("Failed get file size");
	}
	auto res = in.tellg();
	in.close();
	return res;
}

// Cat files
using FPrepareError = function<void(const string& errMessage, bool bResult)>;

void fileConcat(const string& fileName, ostream& ofile,
	istream* pIn = nullptr, FPrepareError* fnPreparedErr = nullptr)
{
	// Function stub for
	static FPrepareError prepErrStub = [](const string& errMessage, bool bResult) {
		error(errMessage);
	};
	// Configure prepared function
	auto&& fnDefaultErr = fnPreparedErr ? *fnPreparedErr : prepErrStub;
	auto size = fileSize(fileName);
	auto& bufSize = Global::READ_FILE_BUFSIZE;

	// Prepared in reference
	auto inRef = std::ref(*pIn);
	Deleter deleter;
	if (!pIn) {
		// Create file if omit input stream
		pIn = new ifstream{ fileName };
		deleter.ptr = pIn;
		inRef = (*pIn);
	}
	auto& in = inRef.get();

	// Creating buffer and parts count
	ios::streampos parts = size / bufSize;
	char* buf = new char[static_cast<size_t>(bufSize)];
	DeleterArray deleterAr{ buf };

	auto partSize = bufSize;
	for (std::streampos i = 0; i <= parts; i += 1) {
		// Change read/write size
		if (i == parts) {
			partSize = size - parts * bufSize;
		}

		// Trying read data
		in.read(&buf[0], partSize);
		if (in.bad() || in.fail()) {
			fnDefaultErr("Failed read from file: " + fileName, false);
		}

		// Trying write data
		ofile.write(&buf[0], size);
		if (ofile.bad() || ofile.fail()) {
			fnDefaultErr("Failed write to file: " + fileName, false);
		}

		// Set next position
		auto curPos = in.tellg();
		curPos += partSize;
		in.seekg(curPos);
	}
};
//// end Files

//// Randoms
bool randInit() {
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	return true;
}

int randInt() {
	int res = 0;
	if (RAND_MAX == INT_MAX) {
		res = rand();
	}
	else {
		// Bits into RAND_MAX
		static const uint8_t nbit = reqBits(RAND_MAX);
		// Bits into int unsigned part
		static const uint8_t iint = sizeof(int) * 8;
		// How much nbits into INT_MAX
		static const uint8_t gbit = iint / nbit;
		// Rest bits
		static const uint8_t obit = iint - gbit * nbit;
		// Max number by rest bits
		static const auto maxNum = maxNumByBits(obit);
		for (uint8_t i = 0; i < gbit * nbit; i += nbit) {
			int curRnd = rand();
			res |= (curRnd << i);
		}
		if (obit > 0) {
			int restRnd = rand();
			res |= (restRnd % maxNum) << gbit * nbit;
		}
	}
	return res;
}