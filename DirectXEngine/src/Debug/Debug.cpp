#include "Debug.h"

#include "../Util.h"
#include "../Entity.h"
#include "../Scene/Scene.h"

SceneDebugWindow* Debug::sceneDebugWindow = nullptr;
EntityDebugWindow* Debug::entityDebugWindow = nullptr;
AssetDebugWindow* Debug::assetDebugWindow = nullptr;

bool Debug::inPlayMode = false;

TwStructMember Debug::vec3Members[] =
{
	{ "x", TW_TYPE_FLOAT, 0, " step=0.1 " },
	{ "y", TW_TYPE_FLOAT, sizeof(float), " step=0.1 " },
	{ "z", TW_TYPE_FLOAT, sizeof(float) * 2, " step=0.1 " }
};
TwType Debug::TW_TYPE_VEC3F = TW_TYPE_UNDEF;
TwType Debug::TW_TYPE_VEC2F = TW_TYPE_UNDEF;

void Debug::createConsoleWindow()
{
#if defined(DEBUG) || defined(_DEBUG)
	createConsoleWindow(500, 120, 32, 120);
	Debug::message("Console window created successfully.");
#endif
}

void Debug::initDebugWindows(ID3D11Device* device, int width, int height)
{
	int success = TwInit(TW_DIRECT3D11, device);
	if (!success)
	{
		const char* errorMessage = TwGetLastError();
		warning("Failed to initialize AntTweakBar: " + std::string(errorMessage));
		warning("No debug controls will be available.");
	}

	TwCopyStdStringToClientFunc(&copyStringToClient);
	TwWindowSize(width, height);
	TwDefine(" GLOBAL iconpos=topleft iconalign=horizontal contained=true ");

	TW_TYPE_VEC3F = TwDefineStruct("TW_TYPE_VEC3F", vec3Members, 3, sizeof(float) * 3, nullptr, nullptr);
	TW_TYPE_VEC2F = TwDefineStruct("TW_TYPE_VEC2F", vec3Members, 2, sizeof(float) * 2, nullptr, nullptr);
	
	sceneDebugWindow = new SceneDebugWindow("scene", "Scene");
	entityDebugWindow = new EntityDebugWindow("entities", "Entity List");
	assetDebugWindow = new AssetDebugWindow("assets", "Assets");
}

void Debug::cleanDebugWindows()
{
	delete sceneDebugWindow;
	delete entityDebugWindow;
	delete assetDebugWindow;

	TwTerminate();
}

void Debug::message(std::string message)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::cout << message << std::endl;
#endif
}

void Debug::warning(std::string warning)
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

void Debug::error(std::string error)
{
#if defined(DEBUG) || defined(_DEBUG)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	WORD oldColorAttrs = csbi.wAttributes;

	// 12 = Red text
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << error << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), oldColorAttrs);

	DebugBreak();
#endif
}

// --------------------------------------------------------
// Allocates a console window we can print to for debugging
// 
// bufferLines   - Number of lines in the overall console buffer
// bufferColumns - Numbers of columns in the overall console buffer
// windowLines   - Number of lines visible at once in the window
// windowColumns - Number of columns visible at once in the window
// --------------------------------------------------------
void Debug::createConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
{
	// Our temp console info struct
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console info and set the number of lines
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferLines;
	coninfo.dwSize.X = bufferColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

Debug::Debug()
{
}

Debug::~Debug()
{
}
