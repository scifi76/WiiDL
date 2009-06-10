#pragma once
#include <string>
#include <algorithm>

using namespace std;
///<summary>
/// Contains utility functions
///</summary>
class Utils
{
public:
	Utils(void);
	~Utils(void);

	static string GetFileExtension(string inputFileName);
	static string StringToUpper(string inputString);
};
