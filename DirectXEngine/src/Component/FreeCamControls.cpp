#include "FreeCamControls.h"

#include "Transform.h"
#include "../Input.h"

using namespace DirectX;

FreeCamControls::FreeCamControls(Entity& entity) : Component(entity)
{
	moveSpeedSlow = 1.0f;
	moveSpeedNormal = 5.0f;
	moveSpeedFast = 20.0f;
	moveSpeed = moveSpeedNormal;
}

FreeCamControls::~FreeCamControls()
{
}

void FreeCamControls::initDebugVariables()
{
	Component::initDebugVariables();

	Debug::entityDebugWindow->addVariable(&moveSpeedFast, TW_TYPE_FLOAT, "Fast Move Speed", this, "", " min=0 step=0.1 ");
	Debug::entityDebugWindow->addVariable(&moveSpeedNormal, TW_TYPE_FLOAT, "Move Speed", this, "", " min=0 step=0.1 ");
	Debug::entityDebugWindow->addVariable(&moveSpeedSlow, TW_TYPE_FLOAT, "Slow Move Speed", this, "", " min=0 step=0.1 ");
}

void FreeCamControls::update(float deltaTime, float totalTime)
{
	if (Input::isKeyDown(Keyboard::LeftShift))
	{
		moveSpeed = moveSpeedFast;
	}
	else if (Input::isKeyDown(Keyboard::LeftControl))
	{
		moveSpeed = moveSpeedSlow;
	}
	else
	{
		moveSpeed = moveSpeedNormal;
	}

	Transform* transform = entity.getComponent<Transform>();

	if (Input::isKeyDown(Keyboard::W))
	{
		transform->moveLocalZ(moveSpeed * deltaTime);
	}

	if (Input::isKeyDown(Keyboard::S))
	{
		transform->moveLocalZ(-moveSpeed * deltaTime);
	}

	if (Input::isKeyDown(Keyboard::A))
	{
		transform->moveLocalX(-moveSpeed * deltaTime);
	}

	if (Input::isKeyDown(Keyboard::D))
	{
		transform->moveLocalX(moveSpeed * deltaTime);
	}

	if (Input::isKeyDown(Keyboard::X))
	{
		transform->moveY(-moveSpeed * deltaTime);
	}

	if (Input::isKeyDown(Keyboard::Space))
	{
		transform->moveY(moveSpeed * deltaTime);
	}

	if (Input::isMouseButtonDown(MOUSE_RIGHT))
	{
		XMFLOAT2 mouseDelta = Input::getMouseDelta();

		Transform* transform = entity.getComponent<Transform>();
		transform->rotateLocal(XMFLOAT3(mouseDelta.y / 10.0f, mouseDelta.x / 10.0f, 0.0f));
	}
}

void FreeCamControls::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator speedFast = dataObject.FindMember("moveSpeedFast");
	rapidjson::Value::MemberIterator speedNormal = dataObject.FindMember("moveSpeedNormal");
	rapidjson::Value::MemberIterator speedSlow = dataObject.FindMember("moveSpeedSlow");

	if (speedFast != dataObject.MemberEnd())
	{
		moveSpeedFast = speedFast->value.GetFloat();
	}

	if (speedNormal != dataObject.MemberEnd())
	{
		moveSpeedNormal = speedNormal->value.GetFloat();
	}

	if (speedSlow != dataObject.MemberEnd())
	{
		moveSpeedSlow = speedSlow->value.GetFloat();
	}
}

void FreeCamControls::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("moveSpeedFast");
	writer.Double(moveSpeedFast);

	writer.Key("moveSpeedNormal");
	writer.Double(moveSpeedNormal);

	writer.Key("moveSpeedSlow");
	writer.Double(moveSpeedSlow);
}
