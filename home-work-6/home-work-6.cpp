// home-work-6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "utils.h"

#include <iostream>
#include <cmath>
#include <ctime>
#include <functional>
#include <conio.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <type_traits>

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::nothrow;
using std::string;
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

using std::ios;

template<typename T>
void outArray(T* elem, size_t arSize, uint8_t space = 2) {
    ios state(nullptr);
    state.copyfmt(cout);
    if (arSize == 0)
        return;
    auto align = getDigits10<T>() + space;
    cout
		<< std::left
		<< std::setw(align);
    cout << elem[0];
    for(size_t i = 1; i < arSize; ++i)
        cout 
		    << std::left
		    << std::setw(align)
            << elem[i];
    cout << endl;

    cout.copyfmt(state);
}

template<typename T>
void outArray(T** arOfAr, size_t m, size_t n) {
    for(size_t i = 0; i < m; ++i)
        outArray(arOfAr[i], n);
}

template<typename T>
void askValue(const string& reqMsg, T& res) {
	cout << reqMsg;
	cin >> res;
	cin.clear();
	cin.ignore(CIN_MAX, '\n');
}

size_t askArraySize() {
    size_t res;
    askValue("Please enter array of integer size: ", res);
    return res;
}

string askFile(const string& reqMsg) {
    string res;
    askValue(reqMsg, res);
    return res;
}

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
void fillMatrix(T** ar, size_t m, size_t n, function<T()> fnGenerator) {
    for (size_t i = 0; i < m; ++i) {
        for(size_t j = 0; j < n; ++j)
            ar[i][j] = fnGenerator();
    }
}

void fillStream1(ostream& out) {
    randInit();
    ios state(nullptr);
    state.copyfmt(out);
    out << std::hex;
    for(int i = 0; i < 5; ++i) {
        for (int j = 0; j < 18; ++j) {
			if (j == 8 || j == 9)
				out << ' ';
			else {
				int curRand = rand() % 16;
                out << curRand;
            }
        }
        out << endl;
    }

    out.copyfmt(state);
}

void fillStream2(ostream& out) {
    randInit();
    static char genomAlphabet[] = {'U', 'C', 'A', 'G'};
    for(int i = 0; i < 100; ++i)
        out << genomAlphabet[rand() % sizeof(genomAlphabet)];
}

static auto fnAskFile = [](const string& askMsg, string& file) {
	do {
		file = askFile(askMsg);
	} while (file.empty());
};



void catFiles(const string& resFile, const string& catFile1, const string& catFile2) {
	ofstream ofile(resFile, ios::binary);
    ifstream in1(catFile1, ios::binary);
    ifstream in2(catFile2, ios::binary);

    bool bWrited = true;
    string errMsg;

    //// Lambda helpers
    // Close all files
    auto closeFiles = [&ofile, &in1, &in2]() {
		ofile.close();
		in1.close();
		in2.close();
    };

    // Check successful write
    auto fnCheckWrite = [&bWrited, &errMsg, &closeFiles]() {
        if (!bWrited) {
            closeFiles();
            error(errMsg);
            return false;
        }
        return true;
    };
    
    //// end Lambda helpers

    // Define function for preparing error info
	static FPrepareError prepareErr =
		[&errMsg, &bWrited](const string& errMessage, bool bResult)
    {
		errMsg = errMessage;
        bWrited = bResult;
	};

    // Main logic
    fileConcat(catFile1, ofile, &in1, &prepareErr);
    if (!fnCheckWrite()) return;
	fileConcat(catFile2, ofile, &in2, &prepareErr);
	if (!fnCheckWrite()) return;
    closeFiles();
}

void task1() {
    cout << "===== Task 1 =====\n";
	size_t arSize = askArraySize();
    int* arInts = new (nothrow) int[arSize];
    Deleter deleter{arInts};
    if (!arInts) 
        error("Failed alloc memory for arInts");
    else {
		for (size_t i = 0; i < arSize; ++i) {
			arInts[i] = static_cast<int>(pow(2, i));
		}
        outArray(arInts, arSize);
    }
    cout << endl;
}

void task2() {
    cout << "===== Task 2 =====\n";
    size_t rows = 4, cols = 4;
    auto matrix = allocMatrix<int>(rows, cols);
    DeleterMatrix deleter(matrix, rows);
    function<int()> fn = []() {
        static bool dontuse = randInit();
        return randInt();
    };
    fillMatrix(matrix, rows, cols, fn);
    outArray(matrix, rows, cols);
    cout << endl;
}

void task3() {
    
    cout << "===== Task 3 =====\n";
    string file1, file2;
    fnAskFile("Please type file1: ", file1);
    fnAskFile("Please type file2: ", file2);

    file1 += Global::txtExt;
    file2 += Global::txtExt;
    Global::file1 = file1;
    Global::file2 = file2;

    fillFile(file1, fillStream1);
    fillFile(file2, fillStream2);
}



void task4() {
    string resFile;
    fnAskFile("Enter result file: ", resFile);
    resFile += Global::txtExt;
    catFiles(resFile, Global::file1, Global::file2);
}
int main()
{
    //task1();
    //task2();
    task3();
    task4();
    pauseExit();
}
