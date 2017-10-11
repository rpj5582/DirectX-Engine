#include "FreeCamControls.h"

#include "Scene.h"

using namespace DirectX;

FreeCamControls::FreeCamControls(Scene* scene, Entity entity) : Component(scene, entity)
{
	transform = nullptr;

	moveSpeedSlow = 1.0f;
	moveSpeedNormal = 5.0f;
	moveSpeedFast = 20.0f;
	moveSpeed = moveSpeedNormal;

	freeLook = false;
	prevMousePos = POINT();
}

FreeCamControls::~FreeCamControls()
{
	scene->unsubscribeMouseDown(this);
	scene->unsubscribeMouseUp(this);
	scene->unsubscribeMouseMove(this);
}

void FreeCamControls::init()
{
	transform = scene->getComponentOfEntity<Transform>(entity);

	scene->subscribeMouseDown(this);
	scene->subscribeMouseUp(this);
	scene->subscribeMouseMove(this);
}

void FreeCamControls::update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_LSHIFT) && 0x8000)
	{
		moveSpeed = moveSpeedFast;
	}
	else if (GetAsyncKeyState(VK_LCONTROL) && 0x8000)
	{
		moveSpeed = moveSpeedSlow;
	}
	else
	{
		moveSpeed = moveSpeedNormal;
	}

	if (GetAsyncKeyState('W') && 0x8000)
	{
		transform->moveLocalZ(moveSpeed * deltaTime);
	}

	if (GetAsyncKeyState('S') && 0x8000)
	{
		transform->moveLocalZ(-moveSpeed * deltaTime);
	}

	if (GetAsyncKeyState('A') && 0x8000)
	{
		transform->moveLocalX(-moveSpeed * deltaTime);
	}

	if (GetAsyncKeyState('D') && 0x8000)
	{
		transform->moveLocalX(moveSpeed * deltaTime);
	}

	if (GetAsyncKeyState('X') && 0x8000)
	{
		transform->moveY(-moveSpeed * deltaTime);
	}

	if (GetAsyncKeyState(VK_SPACE) && 0x8000)
	{
		transform->moveY(moveSpeed * deltaTime);
	}
}

void FreeCamControls::onMouseDown(WPARAM buttonState, int x, int y)
{
	if (buttonState == MK_LBUTTON)
	{
		freeLook = true;

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

void FreeCamControls::onMouseUp(WPARAM buttonState, int x, int y)
{
	freeLook = false;
}

void FreeCamControls::onMouseMove(WPARAM buttonState, int x, int y)
{
	if (freeLook)
	{
		float mouseDeltaX = x - (float)prevMousePos.x;
		float mouseDeltaY = y - (float)prevMousePos.y;

		transform->rotateLocal(XMFLOAT3(mouseDeltaY / 10.0f, mouseDeltaX / 10.0f, 0.0f));

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}
