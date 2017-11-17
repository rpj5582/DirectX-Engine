#include "LightComponent.h"

#include "Transform.h"
#include "../Scene/Scene.h"

using namespace DirectX;

TwStructMember LightComponent::d_lightStructMembers[5] = {
	{ "color", TW_TYPE_COLOR3F, 0, " label='Color' " },
	{ "brightness", TW_TYPE_FLOAT, sizeof(XMFLOAT4), " label='Brightness' min=0 step=0.1 " },
	{ "specularity", TW_TYPE_FLOAT, sizeof(XMFLOAT4) + sizeof(float), " label='Specularity' min=0 step=0.1 " },
	{ "radius", TW_TYPE_FLOAT, sizeof(XMFLOAT4) + sizeof(float) * 2, " label='Radius' min=0 step=0.1 " },
	{ "spotAngle", TW_TYPE_FLOAT, sizeof(XMFLOAT4) + sizeof(float) * 3, " label='Spot Light Angle' min=0 max=90 step=0.1 " }
};

TwType LightComponent::TW_TYPE_LIGHT_SETTINGS = TW_TYPE_UNDEF;

TwEnumVal LightComponent::d_lightTypeMembers[] = 
{
	{ DIRECTIONAL_LIGHT, "Directional Light" },
	{ POINT_LIGHT, "Point Light" },
	{ SPOT_LIGHT, "Spot Light" }
};
TwType LightComponent::TW_TYPE_LIGHT_TYPE = TW_TYPE_UNDEF;

LightComponent::LightComponent(Entity& entity) : Component(entity)
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());

	castShadows = false;
}

LightComponent::~LightComponent()
{
	if (!entity.getComponent<LightComponent>())
	{
		if (entity.hasTag(TAG_LIGHT))
			entity.removeTag(TAG_LIGHT);
	}
}

void LightComponent::init()
{
	Component::init();

	if (!entity.hasTag(TAG_LIGHT))
		entity.addTag(TAG_LIGHT);

	setLightType(DIRECTIONAL_LIGHT);
	setSettingsDefault();
}

void LightComponent::initDebugVariables()
{
	Component::initDebugVariables();

	if (TW_TYPE_LIGHT_SETTINGS == TW_TYPE_UNDEF)
		TW_TYPE_LIGHT_SETTINGS = TwDefineStruct("TW_TYPE_LIGHT_SETTINGS", d_lightStructMembers, 5, sizeof(LightSettings), nullptr, nullptr);

	if (TW_TYPE_LIGHT_TYPE == TW_TYPE_UNDEF)
		TW_TYPE_LIGHT_TYPE = TwDefineEnum("TW_TYPE_LIGH_TYPE", d_lightTypeMembers, 3);

	Debug::entityDebugWindow->addVariable(&m_light, TW_TYPE_LIGHT_SETTINGS, "Light Settings", this);
	Debug::entityDebugWindow->addVariable(&m_lightType, TW_TYPE_LIGHT_TYPE, "Light Type", this);
	Debug::entityDebugWindow->addVariable(&castShadows, TW_TYPE_BOOLCPP, "Cast Shadows", this);
}

void LightComponent::lateUpdate(float deltaTime, float totalTime)
{
	updateViewMatrix();
}

void LightComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator type = dataObject.FindMember("type");
	if (type != dataObject.MemberEnd())
	{
		std::string lightTypeString = type->value.GetString();

		switch (stringHash(lightTypeString.c_str()))
		{
		case stringHash("point"):
			m_lightType = POINT_LIGHT;
			break;

		case stringHash("directional"):
			m_lightType = DIRECTIONAL_LIGHT;
			break;

		case stringHash("spot"):
			m_lightType = SPOT_LIGHT;
			break;

		default:
			Debug::warning("Invalid light type " + lightTypeString + " on LightComponent of entity " + entity.getName() + ", treating as a directional light.");
			break;
		}
	}

	rapidjson::Value::MemberIterator color = dataObject.FindMember("color");
	rapidjson::Value::MemberIterator brightness = dataObject.FindMember("brightness");
	rapidjson::Value::MemberIterator specularity = dataObject.FindMember("specularity");
	rapidjson::Value::MemberIterator radius = dataObject.FindMember("radius");
	rapidjson::Value::MemberIterator spotAngle = dataObject.FindMember("spotAngle");

	if (color != dataObject.MemberEnd())
	{
		m_light.color = XMFLOAT4(color->value["r"].GetFloat(), color->value["g"].GetFloat(), color->value["b"].GetFloat(), color->value["a"].GetFloat());
	}

	if (brightness != dataObject.MemberEnd())
	{
		m_light.brightness = brightness->value.GetFloat();
	}

	if (specularity != dataObject.MemberEnd())
	{
		m_light.specularity = specularity->value.GetFloat();
	}

	if (radius != dataObject.MemberEnd())
	{
		m_light.radius = radius->value.GetFloat();
	}

	if (spotAngle != dataObject.MemberEnd())
	{
		m_light.spotAngle = spotAngle->value.GetFloat();
	}

	rapidjson::Value::MemberIterator castShadowsIter = dataObject.FindMember("castShadows");
	if (castShadowsIter != dataObject.MemberEnd())
	{
		castShadows = castShadowsIter->value.GetBool();
	}
}

void LightComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("type");
	switch (m_lightType)
	{
	case POINT_LIGHT:
		writer.String("point");
		break;

	case DIRECTIONAL_LIGHT:
		writer.String("directional");
		break;

	case SPOT_LIGHT:
		writer.String("spot");
		break;

	default:
		Debug::warning("Invalid light type " + std::to_string(m_lightType) + " on LightComponent of entity " + entity.getName() + ", saving as a directional light.");
		writer.String("directional");
		break;
	}

	writer.Key("color");
	writer.StartObject();

	writer.Key("r");
	writer.Double(m_light.color.x);

	writer.Key("g");
	writer.Double(m_light.color.y);

	writer.Key("b");
	writer.Double(m_light.color.z);

	writer.Key("a");
	writer.Double(m_light.color.w);

	writer.EndObject();

	writer.Key("brightness");
	writer.Double(m_light.brightness);

	writer.Key("specularity");
	writer.Double(m_light.specularity);

	writer.Key("radius");
	writer.Double(m_light.radius);

	writer.Key("spotAngle");
	writer.Double(m_light.spotAngle);

	writer.Key("castShadows");
	writer.Bool(castShadows);
}

LightType LightComponent::getLightType() const
{
	return m_lightType;
}

void LightComponent::setLightType(LightType lightType)
{
	m_lightType = lightType;
	updateProjectionMatrix(NEAR_Z, FAR_Z);
}

LightSettings LightComponent::getLightSettings() const
{
	return m_light;
}

void LightComponent::setLightSettings(const LightSettings& settings)
{
	m_light = settings;
}

void LightComponent::useDefaultSettings()
{
	setSettingsDefault();
}

DirectX::XMFLOAT4X4 LightComponent::getViewMatrix() const
{
	return m_viewMatrix;
}

DirectX::XMFLOAT4X4 LightComponent::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

void LightComponent::setSettingsDefault()
{
	m_light.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_light.brightness = 1.0f;
	m_light.specularity = 32.0f;
	m_light.radius = 1.0f;
	m_light.spotAngle = 15.0f;
}

void LightComponent::updateViewMatrix()
{
	Transform* transform = entity.getComponent<Transform>();
	if (!transform) return;

	switch (m_lightType)
	{
	case POINT_LIGHT:
	{
		Debug::error("Point light view matrices are not yet implemented.");
		XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
		break;
	}

	case DIRECTIONAL_LIGHT:
	{
		CameraComponent* mainCamera = entity.getScene().getMainCamera();
		if (!mainCamera)
		{
			Debug::warning("No main camera - could not build valid view matrix for directional light entity " + entity.getName());
			XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
			return;
		}
		Transform* mainCameraTransform = mainCamera->getEntity().getComponent<Transform>();
		if (!mainCameraTransform)
		{
			Debug::warning("Main camera does not have a transform component - could not build valid view matrix for directional light entity " + entity.getName());
			XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
			return;
		}

		XMFLOAT3 mainCameraPosition = mainCameraTransform->getPosition();
		XMVECTOR eye = XMLoadFloat3(&mainCameraPosition);

		XMFLOAT3 forwardFloat3 = transform->getForward();
		XMVECTOR forward = XMLoadFloat3(&forwardFloat3);

		XMFLOAT3 upFloat3 = transform->getUp();
		XMVECTOR up = XMLoadFloat3(&upFloat3);

		eye = XMVectorMultiplyAdd(forward, XMVectorReplicate(-10.0f), eye);

		XMMATRIX viewMatrix = XMMatrixLookToLH(eye, forward, up);
		XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
		break;
	}

	case SPOT_LIGHT:
	{
		XMFLOAT3 position = transform->getPosition();

		XMFLOAT3 forwardFloat3 = transform->getForward();
		XMFLOAT3 upFloat3 = transform->getUp();

		XMVECTOR eye = XMLoadFloat3(&position);
		XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
		XMVECTOR up = XMLoadFloat3(&upFloat3);

		XMMATRIX viewMatrix = XMMatrixLookToLH(eye, forward, up);
		XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
		break;
	}

	default:
	{
		Debug::error("Invalid light type " + std::to_string(m_lightType));
		break;
	}
	}
}

void LightComponent::updateProjectionMatrix(float nearZ, float farZ, float width, float height)
{
	switch (m_lightType)
	{
	case SPOT_LIGHT:
	case POINT_LIGHT:
	{
		XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)Window::getWidth() / Window::getHeight(), nearZ, farZ);
		XMStoreFloat4x4(&m_projectionMatrix, perspective);
		break;
	}
		

	case DIRECTIONAL_LIGHT:
	{
		XMMATRIX orthographic = XMMatrixOrthographicLH(width, height, nearZ, farZ);
		XMStoreFloat4x4(&m_projectionMatrix, orthographic);
		break;
	}

	default:
		break;
	}

	
}
