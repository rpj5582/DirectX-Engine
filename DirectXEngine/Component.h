#pragma once

#include "Entity.h"
#include "AssetManager.h"

#include "rapidjson\document.h"
#include "rapidjson\writer.h"

#include "Util.h"

#include <Windows.h>
#include <DirectXMath.h>

class Component
{
public:
	friend class Entity;

	virtual void init();
	virtual void update(float deltaTime, float totalTime);
	virtual void lateUpdate(float deltaTime, float totalTime);
	virtual void loadFromJSON(rapidjson::Value& dataObject);
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	Entity& getEntity() const;
	std::string getName() const;

	bool enabled;
	
protected:
	Component(Entity& entity);
	virtual ~Component();

	virtual void onMouseDown(WPARAM buttonState, int x, int y);
	virtual void onMouseUp(WPARAM buttonState, int x, int y);
	virtual void onMouseMove(WPARAM buttonState, int x, int y);
	virtual void onMouseWheel(float wheelDelta, int x, int y);

	Entity& entity;	
	std::string typeName;
};