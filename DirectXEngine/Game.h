#pragma once

#include "DXCore.h"

#include "Entity.h"
#include "LightComponent.h"

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
	std::vector<LightComponent*> m_lights; // A separate list for lights
	// Since I want everything in the engine to be an entity with components, I make
	// A separate list that contains all light components. The light entities will
	// also be in the original entity list however, so I'm only deleting
	// the original entity list. In the future, I'll have a "scene" class
	// that has functions for adding special entities such as lights
	// to the scene so that it will be easier to use.

	CameraManager* m_cameraManager;

	AssetManager* m_assetManager;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT m_prevMousePos;
};

