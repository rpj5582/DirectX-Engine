#include "GUIDebugSpriteComponent.h"

#include "../Scene/Scene.h"
#include "../Input.h"

using namespace DirectX;

GUIDebugSpriteComponent::GUIDebugSpriteComponent(Entity& entity) : GUISpriteComponent(entity)
{
}

GUIDebugSpriteComponent::~GUIDebugSpriteComponent()
{
}

void GUIDebugSpriteComponent::init()
{
	setTexture(DEBUG_TEXTURE_DEFAULTICON);
}

void GUIDebugSpriteComponent::initDebugVariables()
{
}

void GUIDebugSpriteComponent::update(float deltaTime, float totalTime)
{
	if (Input::isMouseButtonPressed(MOUSE_LEFT))
	{
		XMFLOAT2 mousePosition = Input::getMousePositon();

		if (entity.getDebugIconTransform()->containsPoint(mousePosition))
		{
			entity.selected = !entity.selected;
			Debug::entityDebugWindow->showEntity(&entity);
		}
		else
		{
			entity.selected = false;
			Debug::entityDebugWindow->hideEntity(&entity);
		}
	}
}

void GUIDebugSpriteComponent::loadFromJSON(rapidjson::Value& dataObject)
{
}

void GUIDebugSpriteComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
}

void GUIDebugSpriteComponent::calculatePosition(XMFLOAT3 entityPosition)
{
	CameraComponent* debugCamera = entity.getScene().getDebugCamera();
	if (debugCamera->isVisible(entityPosition))
	{
		if (!enabled)
			enabled = true;

		XMFLOAT2 screenPosition = debugCamera->worldToScreen(entityPosition);
		entity.getDebugIconTransform()->setPosition(screenPosition);
	}
	else
	{
		enabled = false;
	}
}
