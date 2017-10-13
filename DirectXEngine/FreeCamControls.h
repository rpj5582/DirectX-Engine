#pragma once
#include "Component.h"

class FreeCamControls : public Component
{
public:
	FreeCamControls(Entity& entity);
	~FreeCamControls();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	virtual void onMouseDown(WPARAM buttonState, int x, int y) override;
	virtual void onMouseUp(WPARAM buttonState, int x, int y) override;
	virtual void onMouseMove(WPARAM buttonState, int x, int y) override;

private:
	float moveSpeed;
	float moveSpeedSlow;
	float moveSpeedNormal;
	float moveSpeedFast;

	bool freeLook;
	POINT prevMousePos;
};