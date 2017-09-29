#pragma once

#include "Component.h"
#include <DirectXMath.h>

#define MAX_LIGHTS 8

enum LightType
{
	POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT
};

struct LightSettings
{
	DirectX::XMFLOAT4 color;
	float brightness;
	float specularity;
	float radius;
	float spotAngle;
	float spotFalloff;
};

class LightComponent : public Component
{
public:
	LightComponent(Entity* entity);
	~LightComponent();

	virtual void init() override;
	virtual void update() override;

	LightType getLightType() const;

	// Changes the light type to either a point light, directional light, or spot light.
	// If useDefaults is true, the light's settings will be set to the default settings.
	void setLightType(LightType lightType, bool useDefaults = true);

	LightSettings getLightSettings() const;
	void setLightSettings(const LightSettings& settings);

	// Sets the light's settings back to default.
	void useDefaultSettings();

private:
	void setSettingsDefault();
	LightSettings m_light;
	LightType m_lightType;
};