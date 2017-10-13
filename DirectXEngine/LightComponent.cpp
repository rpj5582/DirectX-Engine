#include "LightComponent.h"

using namespace DirectX;

LightComponent::LightComponent(Entity& entity) : Component(entity)
{
}

LightComponent::~LightComponent()
{
}

void LightComponent::init()
{
	setLightType(DIRECTIONAL_LIGHT);
	setSettingsDefault();
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
			Output::Warning("Invalid light type " + lightTypeString + " on LightComponent of entity " + entity.getName() + ", treating as a directional light.");
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
		Output::Warning("Invalid light type " + std::to_string(m_lightType) + " on LightComponent of entity " + entity.getName() + ", saving as a directional light.");
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
}

LightType LightComponent::getLightType() const
{
	return m_lightType;
}

void LightComponent::setLightType(LightType lightType)
{
	m_lightType = lightType;
}

LightSettings LightComponent::getLightSettings() const
{
	return m_light;
}

void LightComponent::setLightSettings(const LightSettings& settings)
{
	m_light.color = settings.color;
	m_light.brightness = settings.brightness;
	m_light.specularity = settings.specularity;
	m_light.radius = settings.radius;
	m_light.spotAngle = settings.spotAngle;
}

void LightComponent::useDefaultSettings()
{
	setSettingsDefault();
}

void LightComponent::setSettingsDefault()
{
	m_light.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_light.brightness = 1.0f;
	m_light.specularity = 32.0f;
	m_light.radius = 1.0f;
	m_light.spotAngle = 15.0f;
}
