#include "Game.h"

#include "Scene/Scene1.h"
#include "Scene/Scene2.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		"DirectX Game",	   // Text for the window's title bar
		true)			   // Show extra stats (fps) in title bar?
{
	m_assetManager = nullptr;
	m_sceneManager = nullptr;
	m_input = nullptr;

	Debug::createConsoleWindow();
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	Debug::cleanDebugWindows();

	delete m_assetManager;
	delete m_sceneManager;
	delete m_input;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
HRESULT Game::Init()
{
	HRESULT hr = DXCore::Init();
	if (FAILED(hr))
	{
		return hr;
	}

	Debug::initDebugWindows(device, m_window->getWidth(), m_window->getHeight());

	ComponentRegistry registry;
	registry.registerComponents();

	m_assetManager = new AssetManager(device, context);
	if (!m_assetManager->init()) return E_ABORT;

	m_input = new Input(m_window->getWindowHandle());

	m_sceneManager = new SceneManager();
	m_sceneManager->addScene(new Scene1(device, context, "testscene", "Assets/Scenes/testscene.json"));
	m_sceneManager->addScene(new Scene2(device, context, "testscene2", "Assets/Scenes/testscene2.json"));

	if (m_sceneManager->getSceneCount() == 0)
	{
		Debug::error("No scenes added to the scene list!");
		return E_ABORT;
	}
	else
	{
		m_sceneManager->loadScene(0U);
	}

	return S_OK;
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	m_input->update();

	// Quit if the escape key is pressed
	if (Input::isKeyPressed(Keyboard::Escape))
		Quit();

	if (Input::isKeyPressed(Keyboard::F5))
		Debug::inPlayMode = !Debug::inPlayMode;

	Scene* activeScene = m_sceneManager->getActiveScene();
	if (activeScene)
	{
		activeScene->update(deltaTime, totalTime);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	Scene* activeScene = m_sceneManager->getActiveScene();
	if (activeScene)
	{
		activeScene->render(backBufferRTV, depthStencilView);
	}

	if(!Debug::inPlayMode)
		TwDraw();

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

LRESULT Game::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Pass the message over to AntTweakBar so the UI can react to user input
	if (!Debug::inPlayMode && TwEventWin(m_window->getWindowHandle(), msg, wParam, lParam))
	{
		return 0;
	}

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		Input::ProcessKeyboardMessage(msg, wParam, lParam);
		Input::ProcessMouseMessage(msg, wParam, lParam);

		if (wParam == TRUE)
			Input::resume();
		else
			Input::suspend();
		break;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Input::ProcessMouseMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Input::ProcessKeyboardMessage(msg, wParam, lParam);
		break;
	}

	return DXCore::ProcessMessage(hwnd, msg, wParam, lParam);
}
