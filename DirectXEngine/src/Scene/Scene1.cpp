#include "Scene1.h"

#include "../Component/FreeCamControls.h"
#include "../Component/GUITransform.h"
#include "../Component/GUISpriteComponent.h"

using namespace DirectX;

Scene1::Scene1(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath)
	: Scene(device, context, name, filepath)
{
}

Scene1::~Scene1()
{
}

void Scene1::onLoad()
{
}
