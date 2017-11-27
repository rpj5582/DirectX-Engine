#include "GUIDebugSpriteComponent.h"

#include "../Component/GUITransform.h"
#include "../Component/CameraComponent.h"
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

		DebugEntity* debugIcon = entity.getDebugIcon();
		if (debugIcon->getGUITransform()->containsPoint(mousePosition))
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

void GUIDebugSpriteComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
}

void GUIDebugSpriteComponent::calculatePosition(XMFLOAT3 entityPosition)
{
	DebugEntity* debugIcon = entity.getDebugIcon();

	CameraComponent* debugCamera = entity.getScene().getDebugCamera();
	if (debugCamera->isVisible(entityPosition))
	{
		if (!debugIcon->enabled)
			debugIcon->enabled = true;

		XMFLOAT2 screenPosition = debugCamera->worldToScreen(entityPosition);
		debugIcon->getGUITransform()->setPosition(screenPosition);
	}
	else
	{
		debugIcon->enabled = false;
	}
}
