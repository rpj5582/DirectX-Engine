#pragma once

#include "AssetManager.h"

#include <Windows.h>
#include <DirectXMath.h>

typedef unsigned int Entity;

class Scene;

class Component
{
public:
	friend class Scene;

	virtual void init();
	virtual void update(float deltaTime, float totalTime);

	Entity getEntity() const;

	bool enabled;
	
protected:
	Component(Scene* scene, Entity entity);
	virtual ~Component();

	virtual void onMouseDown(WPARAM buttonState, int x, int y);
	virtual void onMouseUp(WPARAM buttonState, int x, int y);
	virtual void onMouseMove(WPARAM buttonState, int x, int y);
	virtual void onMouseWheel(float wheelDelta, int x, int y);

	Scene* scene;
	Entity entity;
};