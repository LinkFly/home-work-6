#pragma once

#include <string>
#include <iostream>

using std::string;
using std::ios;

namespace Global {
	static string file1;
	static string file2;
	static const string txtExt = ".txt";
	static const ios::streampos READ_FILE_BUFSIZE = 4096;
};
