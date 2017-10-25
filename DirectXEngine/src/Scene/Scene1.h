#pragma once
#include "Scene.h"

#include "../Component/FreeCamControls.h"

class Scene1 : public Scene
{
public:
	Scene1(ID3D11Device* device, ID3D11DeviceContext* context);
	~Scene1();

	virtual void onLoad() override;
	virtual void update(float deltaTime, float totalTime) override;

private:

};