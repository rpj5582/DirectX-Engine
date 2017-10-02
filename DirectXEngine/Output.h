#pragma once
#include <Windows.h>

#include <cstdio>
#include <iostream>
#include <string>

class Output
{
public:
	static void Message(std::string message);
	static void Warning(std::string warning);
	static void Error(std::string error);

private:
	Output();
	~Output();
};
