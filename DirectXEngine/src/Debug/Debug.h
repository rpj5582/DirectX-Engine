#pragma once

#if defined(DEBUG) || defined(_DEBUG)
	#if defined(_WIN64)
	#pragma comment(lib, "AntTweakBar64.lib")
	#else
	#pragma comment(lib, "AntTweakBar.lib")
	#endif
#endif

#include "SceneDebugWindow.h"
#include "EntityDebugWindow.h"
#include "AssetDebugWindow.h"

#include <d3d11.h>
#include <iostream>
#include <string>
#include <unordered_map>

class Scene;
class Entity;

class Debug
{
public:
	static void createConsoleWindow();

	static void message(std::string message);
	static void warning(std::string warning);
	static void error(std::string error);

	static void initDebugWindows(ID3D11Device* device, int width, int height);
	static void cleanDebugWindows();

	static TwType TW_TYPE_VEC3F;
	static TwType TW_TYPE_VEC2F;
	static TwType TW_TYPE_SIZE2;

	static SceneDebugWindow* sceneDebugWindow;
	static EntityDebugWindow* entityDebugWindow;
	static AssetDebugWindow* assetDebugWindow;

#if defined(DEBUG) || defined(_DEBUG)
	static bool inPlayMode;
#endif

private:
	Debug();
	~Debug();

	static void createConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

	static TwStructMember vec3Members[3];
	static TwStructMember sizeMembers[2];
};
