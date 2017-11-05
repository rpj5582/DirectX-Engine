#pragma once

#include "../Entity.h"
#include "../Asset/AssetManager.h"
#include "../Window.h"

#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"

#include "../Util.h"

#include <Windows.h>
#include <DirectXMath.h>

class Component
{
public:
	friend class Entity;

	virtual void init();
	virtual void initDebugVariables();
	virtual void update(float deltaTime, float totalTime);
	virtual void lateUpdate(float deltaTime, float totalTime);
	virtual void loadFromJSON(rapidjson::Value& dataObject);
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer);
	virtual void onSceneLoaded();

	Entity& getEntity() const;
	std::string getName() const;

	bool enabled;
	
protected:
	Component(Entity& entity);
	virtual ~Component();

	Entity& entity;	
	std::string typeName;
};