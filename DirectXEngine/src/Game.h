#pragma once
#include "DXCore.h"

#include "Component/ComponentRegistry.h"
#include "Scene/Scene1.h"

#include "Input.h"

class Game : public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	bool Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	virtual LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	AssetManager* m_assetManager;
	Scene* m_scene;
	Input* m_input;
};

