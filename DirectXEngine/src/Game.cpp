#include "Game.h"

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

	m_renderer = nullptr;
	m_guiRenderer = nullptr;

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
	Debug::destroy();

	delete m_sceneManager;
	delete m_assetManager;

	delete m_guiRenderer;
	delete m_renderer;

	delete m_input;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
HRESULT Game::Init()
{
	Debug::init();

	HRESULT hr = DXCore::Init();
	if (FAILED(hr))
	{
		return hr;
	}

	Debug::lateInit(m_window->getWindowHandle(), device, context);

	m_assetManager = new AssetManager(device, context);
	if (!m_assetManager->init()) return E_ABORT;

	m_renderer = new Renderer(device, context);
	if (!m_renderer->init()) return E_ABORT;

	m_guiRenderer = new GUIRenderer(device, context);
	if (!m_guiRenderer->init()) return E_ABORT;

	ComponentRegistry registry;
	registry.registerComponents();

	m_input = new Input(m_window->getWindowHandle());

	m_sceneManager = new SceneManager(m_window->getWindowHandle());

	TextureParameters editorSceneTextureParameters = {};
	editorSceneTextureParameters.usage = D3D11_USAGE_DEFAULT;
	editorSceneTextureParameters.bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	return S_OK;
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	m_input->update();

	Debug::update();

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
	m_renderer->begin();

	Scene* activeScene = m_sceneManager->getActiveScene();

	activeScene->renderGeometry(m_renderer, backBufferRTV, depthStencilView, (float)Window::getWidth(), (float)Window::getHeight());
	activeScene->renderGUI(m_guiRenderer);

	m_renderer->end();

	Debug::drawGUI();

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


LRESULT Game::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DebugWinMsgReturn result;
	bool shouldProcess = Debug::ProcessMessage(hwnd, msg, wParam, lParam, result);
	if (!shouldProcess) return 0;

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
		if (result.wantMouse) return 0;
		Input::ProcessMouseMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (result.wantKeyboard) return 0;
		Input::ProcessKeyboardMessage(msg, wParam, lParam);
		break;
	}

	return DXCore::ProcessMessage(hwnd, msg, wParam, lParam);
}
