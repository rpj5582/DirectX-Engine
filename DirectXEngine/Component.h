#pragma once

#include <Windows.h>
#include <DirectXMath.h>

class Scene;

class Component
{
public:
	friend class Scene;

	virtual void init() = 0;
	virtual void update(float deltaTime, float totalTime) = 0;

	unsigned int getEntity() const;
	
protected:
	Component(Scene* scene, unsigned int entity);
	~Component();

	virtual void onMouseDown(WPARAM buttonState, int x, int y);
	virtual void onMouseUp(WPARAM buttonState, int x, int y);
	virtual void onMouseMove(WPARAM buttonState, int x, int y);
	virtual void onMouseWheel(float wheelDelta, int x, int y);

	Scene* scene;
	unsigned int entity;
};