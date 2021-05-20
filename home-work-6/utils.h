#pragma once

#include "Global.h"

#include <limits>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace utils {

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::function;
using std::nothrow;
using std::vector;
using std::find_if_not;

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

//// Bits utils
template<typename T>
constexpr uint8_t reqBits(const T num) {
	return static_cast<uint8_t>(ceil(log2(num + 1)));
}

uint64_t maxNumByBits(uint8_t nbits) {
	return static_cast<uint64_t>(pow(2, nbits) - 1);
}

// (ceiling (log (1- (expt 2 (* 8 8))) 10))
template<class T>
constexpr uint8_t getDigits10() {
	auto maxNum = pow(2.0, std::numeric_limits<T>::digits - 1);
	auto res = static_cast<uint8_t>(ceil(log10(maxNum)));
	return res + std::numeric_limits<T>::is_signed;
}
//// end Bits utils

//// Memory, matrix utils
template<typename T, typename TSize = size_t>
T** allocMatrix(TSize m, TSize n) {
	T** res = new (nothrow) T * [m];
	if (!res) {
		error("failed alloc memory for matrix columns");
	}
	else {
		for (size_t i = 0; i < m; ++i) {
			res[i] = new (nothrow) T[n];
			if (!res[i])
				error("failed alloc memory for matrix rows");
		}
	}
	return res;
}

template<typename T>
void freeMatrix(T** ar, size_t m) {
	for (size_t i = 0; i < m; ++i) {
		delete[] ar[i];
	}
	delete[] ar;
}

template<typename T>
void fillMatrix(T** ar, size_t m, size_t n, function<T()> fnGenerator) {
	for (size_t i = 0; i < m; ++i) {
		for (size_t j = 0; j < n; ++j)
			ar[i][j] = fnGenerator();
	}
}
//// end Memory, matrix utils

//// Deleters
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
//// end Deleters

//// Files utils
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

ifstream::pos_type fileSize(istream& in) {
	auto oldPos = in.tellg();
	in.seekg(0, ios::end);
	auto size = in.tellg();
	in.seekg(oldPos, ios::beg);
	return size;
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
	auto size = fileSize(in);

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
//// end Files utils

//// Randoms utils
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
			res |= (restRnd % (maxNum + 1)) << gbit * nbit;
		}
	}
	return res;
}
//// end Randoms utils

// Input utils
template<typename T>
void askValue(const string& reqMsg, T& res) {
	cout << reqMsg;
	cin >> res;
	cin.clear();
	cin.ignore(CIN_MAX, '\n');
}

//// Strings utils
bool checkingForBad(char ch, char* badChars, int badCharsSize) {
	for (int i = 0; i < badCharsSize; ++i) {
		if (ch == badChars[i])
			return true;
	}
	return false;
};

void split(const string& str, vector<string>& strs, char* delims, int delimsSize) {
	string cur;
	for (char ch : str) {
		if (checkingForBad(ch, delims, delimsSize)) {
			if (!cur.empty())
				strs.push_back(cur);
			cur.clear();
		}
		else {
			cur += ch;
		}
	}
	if (!cur.empty())
		strs.push_back(cur);
}

string trim(const string& str, char* badChars, int badCharsSize) {
	auto fnCheckingForBad = [&badChars, badCharsSize](char c) {
		return checkingForBad(c, badChars, badCharsSize);
	};
	auto newStart = find_if_not(str.begin(), str.end(), fnCheckingForBad);
	auto newEnd = find_if_not(str.rbegin(), str.rend(), fnCheckingForBad).base();
	return newEnd <= newStart ? string{} : string{ newStart, newEnd };
}
vector<string> trimAndSplit(const string& str) {
	char badChars[] = { ',', '.', '-', '_', '(', ')', ' ' };
	auto pureStr = trim(str, badChars, sizeof(badChars));
	vector<string> splitted;
	split(pureStr, splitted, badChars, sizeof(badChars));
	return splitted;
}

void downcase(string& str) {
	std::transform(str.begin(), str.end(), str.begin(), [](char c) {
		return std::tolower(c);
		});
}
//// end Strings utils

} // namespace utils