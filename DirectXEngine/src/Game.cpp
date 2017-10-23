#include "Game.h"

#define NEAR_Z 0.1f
#define FAR_Z 100.0f

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
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	m_assetManager = nullptr;
	m_scene = nullptr;
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
	delete m_scene;
	delete m_input;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
bool Game::Init()
{
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Debug::initDebugWindows(device, width, height);

	ComponentRegistry registry;
	registry.registerComponents();

	m_assetManager = new AssetManager(device, context);
	if (!m_assetManager->init()) return false;

	m_input = new Input(hWnd);

	m_scene = new Scene1(device, context);
	if (!m_scene->init()) return false;

	m_scene->updateProjectionMatrix(width, height, NEAR_Z, FAR_Z);

	return true;
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	m_scene->updateProjectionMatrix(width, height, NEAR_Z, FAR_Z);
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

	m_scene->update(deltaTime, totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float backgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	context->ClearRenderTargetView(backBufferRTV, backgroundColor);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_scene->render();

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
	if (!Debug::inPlayMode && TwEventWin(hWnd, msg, wParam, lParam))
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
