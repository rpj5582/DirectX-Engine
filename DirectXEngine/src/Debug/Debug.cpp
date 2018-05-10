#include "Debug.h"

#include "../Third Party/imgui/imgui_impl_dx11.h"
#include "../Input.h"
#include "../Scene/SceneManager.h"

#include <iostream>
#include <d3d11.h>

using namespace DirectX;

bool Debug::inPlayMode = false;

DebugEntityList Debug::m_entityList;
DebugComponentList Debug::m_componentList;
DebugAssetList Debug::m_assetList(nullptr);
DebugConsoleWindow Debug::m_consoleWindow;
DebugMainMenuBar Debug::m_mainMenuBar(m_entityList, m_componentList, m_assetList);

std::unordered_map<unsigned int, const char*> Debug::m_debugEnumString;
std::unordered_map<unsigned int, std::vector<DebugComponentData>> Debug::m_debugStructMembers;

void Debug::init()
{
	ImGui::CreateContext();
}

void Debug::lateInit(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	ImGui_ImplDX11_Init(hWnd, device, context);
	ImGui::StyleColorsDark();

	m_assetList = DebugAssetList(hWnd);
}

void Debug::destroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool Debug::ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DebugWinMsgReturn& output)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		if (m_mainMenuBar.getAppClosedState() == ClosedState::NOT_CLOSED)
		{
			m_mainMenuBar.setAppClosedState(ClosedState::TRYING_TO_CLOSE);
			return false;
		}

		break;
	}
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

	ImGuiIO& io = ImGui::GetIO();
	output.wantKeyboard = io.WantCaptureKeyboard;
	output.wantMouse = io.WantCaptureMouse;
	return true;
}

void Debug::update()
{
	if (!inPlayMode)
	{
		ImGui_ImplDX11_NewFrame();
		//ImGui::ShowDemoWindow();

		// Check if we are trying to close the window, since there may be unsaved changes.
		if (m_mainMenuBar.getAppClosedState() == ClosedState::TRYING_TO_CLOSE)
		{
			bool quitSuccessful = m_mainMenuBar.chooseQuit();
			if (!quitSuccessful)
			{
				m_mainMenuBar.openQuitSaveChangesPopup();
			}
		}

		// Check for shortcuts
		if (Input::isKeyDown(Keyboard::LeftControl))
		{
			if (Input::isKeyDown(Keyboard::LeftShift))
			{
				// Shortcuts with shift

				if (Input::isKeyPressed(Keyboard::S))
				{
					SceneManager::saveActiveSceneAs();
				}
			}
			else
			{
				// Shortcuts without shift

				if (Input::isKeyPressed(Keyboard::N))
				{
					bool successful = m_mainMenuBar.chooseNew();
					if (!successful)
					{
						m_mainMenuBar.openNewSaveChangesPopup();
					}
				}

				if (Input::isKeyPressed(Keyboard::O))
				{
					bool successful = m_mainMenuBar.chooseLoad();
					if (!successful)
					{
						m_mainMenuBar.openLoadSaveChangesPopup();
					}
				}

				if (Input::isKeyPressed(Keyboard::S))
				{
					SceneManager::saveActiveScene();
				}
			}
		}
	}
}

void Debug::drawGUI()
{
	if (!inPlayMode)
	{
		ImGuiIO& io = ImGui::GetIO();

		m_mainMenuBar.draw();

		float mainMenuBarHeight = m_mainMenuBar.getHeight();
		ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(300, io.DisplaySize.y - mainMenuBarHeight));

		if (m_mainMenuBar.shouldDrawEntityList())
			m_entityList.draw();

		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 300, mainMenuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(300, io.DisplaySize.y - mainMenuBarHeight));

		m_componentList.setSelectedEntityID(m_entityList.getSelectedEntity());

		if (m_mainMenuBar.shouldDrawComponentList())
			m_componentList.draw();

		ImGui::SetNextWindowPos(ImVec2(300, io.DisplaySize.y - 300));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2.0f - 300, 300));

		if (m_mainMenuBar.shouldDrawAssetList())
			m_assetList.draw();

		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y - 300));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2.0f - 300, 300));

		if (m_mainMenuBar.shouldDrawConsole())
			m_consoleWindow.draw();

		ImGui::Render();
	}
}

const char* Debug::getEnumDisplayString(unsigned int hash)
{
	if (m_debugEnumString.find(hash) != m_debugEnumString.end())
		return m_debugEnumString.at(hash);
	else
		Debug::error("Enum not registered! Make sure to register the enum in initDebugVariables() by calling Debug::registerEnum() and storing the returned hash value in the DebugComponentData.dataCountOrHash variable.");

	return "";
}

std::vector<DebugComponentData>& Debug::getStructMembers(unsigned int hash)
{
	return m_debugStructMembers.at(hash);
}

void Debug::createConsoleWindow()
{
#if defined(DEBUG) || defined(_DEBUG)
	createConsoleWindow(500, 120, 32, 120);
	Debug::message("Console window created successfully.");
#endif
}

void Debug::message(std::string message)
{
#if defined(DEBUG) || defined(_DEBUG)
	m_consoleWindow.addText(message);
#endif
}

void Debug::warning(std::string warning)
{
#if defined(DEBUG) || defined(_DEBUG)
	m_consoleWindow.addText(warning, 255, 255, 0);
#endif
}

void Debug::error(std::string error)
{
#if defined(DEBUG) || defined(_DEBUG)
	m_consoleWindow.addText(error, 255, 0, 0);
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