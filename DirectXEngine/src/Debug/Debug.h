#pragma once

#include "DebugMainMenuBar.h"
#include "DebugEntityList.h"
#include "DebugComponentList.h"
#include "DebugAssetList.h"
#include "DebugConsoleWindow.h"

#include <d3d11.h>
#include <string>
#include <typeindex>
#include <unordered_map>

struct DebugWinMsgReturn
{
	bool wantMouse;
	bool wantKeyboard;
};

enum DebugTypes
{
	D_INT,
	D_UINT,
	D_FLOAT,
	D_BOOL,
	D_CHAR,
	D_STRING,
	D_ENUM,
	D_STRUCT,
	D_VEC2,
	D_VEC3,
	D_VEC4,
	D_COLOR,
	D_TEXTURE,
	D_MATERIAL,
	D_MODEL,
	D_VERTEX_SHADER,
	D_PIXEL_SHADER,
	D_SAMPLER,
	D_FONT
};

typedef void(*DebugGetterFunc)(const Component*, void*);
typedef void(*DebugSetterFunc)(Component*, const void*);

class Texture;

struct DebugComponentData
{
	std::string label;
	DebugTypes type;
	void* data;
	DebugGetterFunc getterFunc;
	DebugSetterFunc setterFunc;
	unsigned int hash;
	unsigned int stride;
};

class Debug
{
public:
	static void init();
	static void lateInit(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);
	static void destroy();

	static bool ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DebugWinMsgReturn& output);

	static void update();
	static void drawGUI();

	template<typename T>
	static unsigned int registerEnum(const char* displayString);
	static const char* getEnumDisplayString(unsigned int hash);

	template <typename T>
	static unsigned int registerStruct(std::vector<DebugComponentData>& structMembers);
	static std::vector<DebugComponentData>& getStructMembers(unsigned int hash);

	static void createConsoleWindow();

	static void message(std::string message);
	static void warning(std::string warning);
	static void error(std::string error);

#if defined(DEBUG) || defined(_DEBUG)
	static bool inPlayMode;
#endif

private:
	Debug();
	~Debug();

	static void createConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

	static DebugMainMenuBar m_mainMenuBar;
	static DebugEntityList m_entityList;
	static DebugComponentList m_componentList;
	static DebugAssetList m_assetList;
	static DebugConsoleWindow m_consoleWindow;

	static std::unordered_map<unsigned int, const char*> m_debugEnumString;
	static std::unordered_map<unsigned int, std::vector<DebugComponentData>> m_debugStructMembers;
};

template<typename T>
inline unsigned int Debug::registerEnum(const char* displayString)
{
	static_assert(std::is_enum<T>::value, "Given type is not an enum.");

	std::string typeName = typeid(T).name();
	unsigned int hash = Util::stringHash(typeName.c_str());

	m_debugEnumString[hash] = displayString;

	return hash;
}

template<typename T>
inline unsigned int Debug::registerStruct(std::vector<DebugComponentData>& structMembers)
{
	static_assert(std::is_class<T>::value, "Given type is not a struct.");

	std::string typeName = typeid(T).name();
	unsigned int hash = Util::stringHash(typeName.c_str());

	m_debugStructMembers[hash] = std::vector<DebugComponentData>(structMembers);

	return hash;
}
