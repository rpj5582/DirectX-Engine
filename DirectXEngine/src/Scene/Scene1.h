#pragma once
#include "Scene.h"

class Scene1 : public Scene
{
public:
	Scene1(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath);
	~Scene1();

protected:
	void onLoad() override;

private:

};