#include "Game.h"
#include "Vertex.h"

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
	m_renderer = nullptr;
	m_assetManager = nullptr;
	m_scene = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	Output::Message("Console window created successfully.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	delete m_renderer;
	delete m_assetManager;
	delete m_scene;
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

	m_assetManager = new AssetManager(device, context);
	if (!m_assetManager->init()) return false;

	m_scene = new Scene1(device, context);
	if (!m_scene->init()) return false;

	m_scene->updateProjectionMatrix(width, height, NEAR_Z, FAR_Z);

	m_renderer = new Renderer(device, context);
	if (!m_renderer->init()) return false;

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
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	m_scene->update(deltaTime, totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float backgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, backgroundColor);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	LightComponent** lights;
	unsigned int lightCount;
	m_scene->getAllLights(&lights, &lightCount);

	// Preprocess each light entity to get it's position and direction (for forward rendering).
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(MAX_LIGHTS);

	// Only loop through the lights if there is a main camera, since we need its view matrix.
	Camera* mainCamera = m_scene->getMainCamera();
	if (mainCamera)
	{
		for (unsigned int i = 0; i < lightCount; i++)
		{
			//  Don't use disabled components
			if (!lights[i]->enabled) continue;

			const LightSettings lightSettings = lights[i]->getLightSettings();

			// Get position, direction, and type of each light
			Transform* lightTransform = m_scene->getComponentOfEntity<Transform>(lights[i]->getEntity());
			if (lightTransform)
			{
				XMFLOAT3 lightPosition = lightTransform->getPosition();
				XMFLOAT3 lightDirection = lightTransform->getForward();

				unsigned int lightType = (unsigned int)lights[i]->getLightType();

				// Creates the final memory-aligned struct that is sent to the GPU
				lightData[i] = 
				{
					lightSettings.color,
					lightDirection,
					lightSettings.brightness,
					lightPosition,
					lightSettings.specularity,
					lightSettings.radius,
					lightSettings.spotAngle,
					lights[i]->enabled,
					lightType,
				};
			}
		}
	}

	if(lightData.size() > 0)
		m_renderer->render(m_scene, &lightData[0], lightData.size());
	else
		m_renderer->render(m_scene, nullptr, 0);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	SetCapture(hWnd);
	m_scene->onMouseDown(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	ReleaseCapture();
	m_scene->onMouseUp(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	m_scene->onMouseMove(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	m_scene->onMouseWheel(wheelDelta, x, y);
}
#pragma endregion