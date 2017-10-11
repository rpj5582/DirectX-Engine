#pragma once
#include "Scene.h"

class Scene1 : public Scene
{
public:
	Scene1(ID3D11Device* device, ID3D11DeviceContext* context);
	~Scene1();

	virtual bool init() override;

private:

};