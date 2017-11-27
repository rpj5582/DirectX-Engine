#pragma once
#include "Component.h"

class FreeCamControls : public Component
{
public:
	FreeCamControls(Entity& entity);
	~FreeCamControls();

	virtual void initDebugVariables() override;
	virtual void update(float deltaTime, float totalTime) override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

private:
	float moveSpeed;
	float moveSpeedSlow;
	float moveSpeedNormal;
	float moveSpeedFast;
};