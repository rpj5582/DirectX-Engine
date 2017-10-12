#pragma once

#include "Entity.h"
#include "AssetManager.h"

#include "rapidjson\document.h"

#include <Windows.h>
#include <DirectXMath.h>


// Function taken from here: http://rextester.com/YJB48513
static constexpr unsigned int fnv1aBasis = 0x811C9DC5;
static constexpr unsigned int fnv1aPrime = 0x01000193;

constexpr unsigned int stringHash(const char* string, unsigned int h = fnv1aBasis)
{
	return !*string ? h : stringHash(string + 1, static_cast<unsigned int>((h ^ *string) * static_cast<unsigned long long>(fnv1aPrime)));
}

class Component
{
public:
	friend class Entity;

	virtual void init();
	virtual void update(float deltaTime, float totalTime);
	virtual void lateUpdate(float deltaTime, float totalTime);
	virtual void loadFromJSON(rapidjson::Value& dataObject);

	Entity& getEntity() const;

	bool enabled;
	
protected:
	Component(Entity& entity);
	virtual ~Component();

	virtual void onMouseDown(WPARAM buttonState, int x, int y);
	virtual void onMouseUp(WPARAM buttonState, int x, int y);
	virtual void onMouseMove(WPARAM buttonState, int x, int y);
	virtual void onMouseWheel(float wheelDelta, int x, int y);

	Entity& entity;
};