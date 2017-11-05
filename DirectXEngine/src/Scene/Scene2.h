#pragma once
#include "Scene.h"

class Scene2 : public Scene
{
public:
	Scene2(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath);
	~Scene2();

	virtual void onLoad() override;
};

