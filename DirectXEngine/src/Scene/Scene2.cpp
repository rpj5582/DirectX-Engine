#include "Scene2.h"

Scene2::Scene2(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath, unsigned int windowWidth, unsigned int windowHeight, float nearZ, float farZ)
	: Scene(device, context, name, filepath, windowWidth, windowHeight, nearZ, farZ)
{
}

Scene2::~Scene2()
{
}

void Scene2::onLoad()
{

}
