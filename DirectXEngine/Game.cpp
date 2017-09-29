#include "Game.h"
#include "Vertex.h"

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
	m_entities = std::vector<Entity*>();
	m_lights = std::vector<LightComponent*>();

	m_cameraManager = nullptr;
	m_assetManager = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	delete m_assetManager;

	delete m_cameraManager;

	m_lights.clear();

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}
	m_entities.clear();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	m_assetManager = new AssetManager(device, context);

	m_cameraManager = new CameraManager();
	m_cameraManager->updateProjectionMatrix(width, height);

	/*m_lights[0].diffuseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_lights[0].direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	m_lights[0].brightness = 1.0f;
	m_lights[0].position = XMFLOAT3();
	m_lights[0].specularity = 64.0f;
	m_lights[0].radius = 1.0f;
	m_lights[0].spotAngle = 360.0f;
	m_lights[0].spotFalloff = 1.0f;
	m_lights[0].lightType = LightType::DIRECTIONAL_LIGHT;

	m_lights[1].diffuseColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_lights[1].direction = XMFLOAT3();
	m_lights[1].brightness = 0.5f;
	m_lights[1].position = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_lights[1].specularity = 128.0f;
	m_lights[1].radius = 2.0f;
	m_lights[1].spotAngle = 360.0f;
	m_lights[1].spotFalloff = 1.0f;
	m_lights[1].lightType = LightType::POINT_LIGHT;

	m_lights[2].diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_lights[2].direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_lights[2].brightness = 1.0f;
	m_lights[2].position = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_lights[2].specularity = 32.0f;
	m_lights[2].radius = 5.0f;
	m_lights[2].spotAngle = XMConvertToRadians(15.0f);
	m_lights[2].spotFalloff = 2.0f;
	m_lights[2].lightType = LightType::SPOT_LIGHT;*/

	LoadShaders();
	CreateEntities();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	m_assetManager->loadShader("default", VertexShader, L"VertexShader.cso");
	m_assetManager->loadShader("default", PixelShader, L"PixelShader.cso");
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateEntities()
{
	// Create the default material
	Material* defaultMaterial = m_assetManager->addMaterial("default", "default", "default");

	// Create camera entity
	Entity* camera = new Entity(nullptr, defaultMaterial);
	m_entities.push_back(camera);

	camera->move(XMFLOAT3(0.0f, 0.0f, -10.0f));

	m_cameraManager->assignCamera(0, camera); // Assigns this entity to the first camera slot

	Entity* directionalLightEnt = new Entity(nullptr, defaultMaterial);
	directionalLightEnt->rotateLocal(XMFLOAT3(45.0f, 90.0f, 0.0f));
	m_entities.push_back(directionalLightEnt);

	LightComponent* directionalLight = directionalLightEnt->addComponent<LightComponent>();
	directionalLight->setLightType(LightType::DIRECTIONAL_LIGHT);
	m_lights.push_back(directionalLight);

	Entity* spotLightEnt = new Entity(nullptr, defaultMaterial);
	spotLightEnt->moveLocalZ(10.0f);
	spotLightEnt->rotateLocalY(180.0f);
	m_entities.push_back(spotLightEnt);

	LightComponent* spotLight = spotLightEnt->addComponent<LightComponent>();
	spotLight->setLightType(LightType::SPOT_LIGHT);
	m_lights.push_back(spotLight);

	LightSettings spotLightSettings = spotLight->getLightSettings();
	spotLightSettings.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	spotLightSettings.radius = 6.0f;
	spotLightSettings.spotAngle = XMConvertToRadians(2.5f);
	spotLightSettings.spotFalloff = 128.0f;
	spotLight->setLightSettings(spotLightSettings);

	Entity* pointLightEnt = new Entity(nullptr, defaultMaterial);
	m_entities.push_back(pointLightEnt);

	LightComponent* pointLight = pointLightEnt->addComponent<LightComponent>();
	pointLight->setLightType(LightType::POINT_LIGHT);
	m_lights.push_back(pointLight);

	LightSettings pointLightSettings = pointLight->getLightSettings();
	pointLightSettings.color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	pointLight->setLightSettings(pointLightSettings);

	Mesh* cube = m_assetManager->addMesh("cube", "Models/cube.obj");
	Entity* cubeEnt = new Entity(cube, defaultMaterial);
	cubeEnt->moveZ(5.0f);
	cubeEnt->scale(XMFLOAT3(9.0f, 9.0f, 0.0f));
	m_entities.push_back(cubeEnt);

	Mesh* torus = m_assetManager->addMesh("torus", "Models/torus.obj");
	Entity* torusEnt = new Entity(torus, defaultMaterial);
	torusEnt->moveX(2.5f);
	m_entities.push_back(torusEnt);

	Mesh* cone = m_assetManager->addMesh("cone", "Models/cone.obj");
	Entity* coneEnt = new Entity(cone, defaultMaterial);
	coneEnt->moveX(-2.5f);
	coneEnt->scaleY(4.0f);
	m_entities.push_back(coneEnt);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	m_cameraManager->updateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	float cameraSpeed;
	const float cameraSpeedSlow = 1.0f;
	const float cameraSpeedNormal = 5.0f;
	const float cameraSpeedFast = 20.0f;

	if (GetAsyncKeyState(VK_LSHIFT) && 0x8000)
	{
		cameraSpeed = cameraSpeedFast;
	}
	else if(GetAsyncKeyState(VK_LCONTROL) && 0x8000)
	{
		cameraSpeed = cameraSpeedSlow;
	}
	else
	{
		cameraSpeed = cameraSpeedNormal;
	}

	// Test camera controls
	if (GetAsyncKeyState('W') && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveLocalZ(cameraSpeed * deltaTime);
	}

	if (GetAsyncKeyState('S') && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveLocalZ(-cameraSpeed * deltaTime);
	}

	if (GetAsyncKeyState('A') && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveLocalX(-cameraSpeed * deltaTime);
	}

	if (GetAsyncKeyState('D') && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveLocalX(cameraSpeed * deltaTime);
	}

	if (GetAsyncKeyState('X') && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveY(-cameraSpeed * deltaTime);
	}

	if (GetAsyncKeyState(VK_SPACE) && 0x8000)
	{
		m_cameraManager->getCameraEntity(0)->moveY(cameraSpeed * deltaTime);
	}
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

	m_cameraManager->updateViewMatrices();

	// Only uses the first camera slot for now, could be different for split screen, etc.
	XMMATRIX viewMatrix = XMMatrixTranspose(m_cameraManager->getViewMatrix(0));
	XMFLOAT4X4 viewMatrix4x4;
	XMStoreFloat4x4(&viewMatrix4x4, viewMatrix);

	XMMATRIX projectionMatrix = XMMatrixTranspose(m_cameraManager->getProjectionMatrix());
	XMFLOAT4X4 projectionMatrix4x4;
	XMStoreFloat4x4(&projectionMatrix4x4, projectionMatrix);

	// Preprocess each light entity to get it's position and direction
	struct GPU_LIGHT_DATA
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 direction;
		float brightness;
		XMFLOAT3 position;
		float specularity;
		float radius;
		float spotAngle;
		float spotFalloff;
		unsigned int type;
	};
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(m_lights.size());

	for (unsigned int i = 0; i < m_lights.size(); i++)
	{
		const LightSettings lightSettings = m_lights[i]->getLightSettings();

		// Get position, direction, and type of each light
		XMFLOAT3 lightPosition = m_lights[i]->getEntity()->getPosition();
		XMFLOAT3 lightDirection = m_lights[i]->getEntity()->getForward();
		UINT lightType = (UINT)m_lights[i]->getLightType();

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
			lightSettings.spotFalloff,
			lightType
		};
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->getMaterial()->useMaterial();
		SimpleVertexShader* vertexShader = m_entities[i]->getMaterial()->getVertexShader();
		SimplePixelShader* pixelShader = m_entities[i]->getMaterial()->getPixelShader();

		XMMATRIX worldMatrix = XMMatrixTranspose(m_entities[i]->getWorldMatrix());
		XMFLOAT4X4 worldMatrix4x4;
		XMStoreFloat4x4(&worldMatrix4x4, worldMatrix);

		XMMATRIX worldMatrixInverseTranspose = XMMatrixTranspose(m_entities[i]->getWorldMatrixInverseTranspose());
		XMFLOAT4X4 worldMatrixInverseTranspose4x4;
		XMStoreFloat4x4(&worldMatrixInverseTranspose4x4, worldMatrixInverseTranspose);

		vertexShader->SetMatrix4x4("world", worldMatrix4x4);
		vertexShader->SetMatrix4x4("view", viewMatrix4x4);
		vertexShader->SetMatrix4x4("projection", projectionMatrix4x4);
		vertexShader->SetMatrix4x4("worldInverseTranspose", worldMatrixInverseTranspose4x4);
		vertexShader->CopyBufferData("matrices");

		vertexShader->SetFloat3("cameraPosition", m_cameraManager->getCameraEntity(0)->getPosition());
		vertexShader->CopyBufferData("camera");

		int lightCount = lightData.size();
		pixelShader->SetData("lightCount", &lightCount, sizeof(UINT));
		pixelShader->SetData("lights", &lightData[0], sizeof(GPU_LIGHT_DATA) * MAX_LIGHTS);
		pixelShader->CopyBufferData("lighting");

		const Mesh* mesh = m_entities[i]->getMesh();
		if (mesh)
		{
			ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->getIndexBuffer();

			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			// Finally do the actual drawing
			//  - Do this ONCE PER OBJECT you intend to draw
			//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
			//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
			//     vertices in the currently set VERTEX BUFFER
			context->DrawIndexed(
				mesh->getIndexCount(),			// The number of indices to use (we could draw a subset if we wanted)
				0,								// Offset to the first index we want to use
				0);								// Offset to add to each index when looking up vertices
		}
	}

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
	// Save the previous mouse position, so we have it for the future
	m_prevMousePos.x = x;
	m_prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (GetCapture())
	{
		float mouseDeltaX = x - (float)m_prevMousePos.x;
		float mouseDeltaY = y - (float)m_prevMousePos.y;

		m_cameraManager->getCameraEntity(0)->rotateLocal(XMFLOAT3(mouseDeltaY / 10.0f, mouseDeltaX / 10.0f, 0.0f));
	}

	// Save the previous mouse position, so we have it for the future
	m_prevMousePos.x = x;
	m_prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	
}
#pragma endregion