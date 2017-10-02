#include "LightComponent.h"

using namespace DirectX;

LightComponent::LightComponent(Scene* scene, unsigned int entity) : Component(scene, entity)
{
}

LightComponent::~LightComponent()
{
}

void LightComponent::init()
{
}

void LightComponent::update(float deltaTime, float totalTime)
{
}

LightType LightComponent::getLightType() const
{
	return m_lightType;
}

void LightComponent::setLightType(LightType lightType, bool useDefaults)
{
	m_lightType = lightType;

	if (useDefaults)
	{
		setSettingsDefault();
	}
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
	m_light.spotFalloff = settings.spotFalloff;
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
	m_light.spotAngle = XMConvertToRadians(15.0f);
	m_light.spotFalloff = 2.0f;
}
