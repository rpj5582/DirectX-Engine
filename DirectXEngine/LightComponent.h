#pragma once

#include "Component.h"
#include <DirectXMath.h>

#define MAX_LIGHTS 8

// The struct that should match the light data in the shaders.
struct GPU_LIGHT_DATA
{
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
	float brightness;
	DirectX::XMFLOAT3 position;
	float specularity;
	float radius;
	float spotAngle;
	float spotFalloff;
	unsigned int type;
};

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
	LightComponent(Scene* scene, unsigned int entity);
	~LightComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

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