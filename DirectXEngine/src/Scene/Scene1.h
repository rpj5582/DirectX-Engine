#pragma once
#include "Scene.h"

class Scene1 : public Scene
{
public:
	Scene1(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath, unsigned int windowWidth, unsigned int windowHeight, float nearZ, float farZ);
	~Scene1();

	virtual void onLoad() override;
	virtual void update(float deltaTime, float totalTime) override;

private:

};