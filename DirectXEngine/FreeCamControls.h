#pragma once
#include "Component.h"

#include "Transform.h"

class FreeCamControls : public Component
{
public:
	FreeCamControls(Scene* scene, unsigned int entity);
	~FreeCamControls();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	virtual void onMouseDown(WPARAM buttonState, int x, int y) override;
	virtual void onMouseUp(WPARAM buttonState, int x, int y) override;
	virtual void onMouseMove(WPARAM buttonState, int x, int y) override;

private:
	Transform* transform;

	float moveSpeed;
	float moveSpeedSlow;
	float moveSpeedNormal;
	float moveSpeedFast;

	bool freeLook;
	POINT prevMousePos;
};