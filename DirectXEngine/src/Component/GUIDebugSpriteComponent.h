#pragma once
#include "GUISpriteComponent.h"

class GUITransform;

class GUIDebugSpriteComponent : public GUISpriteComponent
{
public:
	GUIDebugSpriteComponent(Entity& entity);
	~GUIDebugSpriteComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void update(float deltaTime, float totalTime) override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	void calculatePosition(DirectX::XMFLOAT3 entityPosition);
};

