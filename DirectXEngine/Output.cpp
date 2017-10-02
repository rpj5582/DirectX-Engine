#include "Output.h"

void Output::Message(std::string message)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::cout << message << std::endl;
#endif
}

void Output::Warning(std::string warning)
{
#if defined(DEBUG) || defined(_DEBUG)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	WORD oldColorAttrs = csbi.wAttributes;

	// 14 = Yellow text
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	std::cout << warning << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), oldColorAttrs);
#endif
}

void Output::Error(std::string error)
{
#if defined(DEBUG) || defined(_DEBUG)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	WORD oldColorAttrs = csbi.wAttributes;

	// 12 = Red text
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << error << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), oldColorAttrs);
#endif
}

Output::Output()
{
}

Output::~Output()
{
}
