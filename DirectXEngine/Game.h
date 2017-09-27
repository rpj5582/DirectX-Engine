#pragma once

#include "DXCore.h"

#include "Lights.h"

#include "Entity.h"
#include "CameraManager.h"

#include "AssetManager.h"

#include <DirectXMath.h>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void CreateEntities();

	std::vector<Entity*> m_entities;

	CameraManager* m_cameraManager;

	AssetManager* m_assetManager;

	Light m_lights[MAX_LIGHTS];
	unsigned int m_lightCount;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT m_prevMousePos;
};

