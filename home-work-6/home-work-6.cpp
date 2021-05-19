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

using namespace utils;

using std::ios;

namespace main_module {
	static string file1;
	static string file2;
	static const string txtExt = ".txt";
}

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

    file1 += main_module::txtExt;
    file2 += main_module::txtExt;
    main_module::file1 = file1;
    main_module::file2 = file2;

    fillFile(file1, fillStream1);
    fillFile(file2, fillStream2);

    cout << endl;
}

void task4() {
    cout << "===== Task 4 =====\n";
    string resFile;
    fnAskFile("Enter result file: ", resFile);
    resFile += main_module::txtExt;
    catFiles(resFile, main_module::file1, main_module::file2);

    cout << endl;
}

void task5() {
	cout << "===== Task 5 =====\n";
	
	

	cout << endl;
}

int main()
{
    task1();
    task2();
	task3();
	task4();
    task5();
    pauseExit();
}
