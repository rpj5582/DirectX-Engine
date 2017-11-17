#pragma once
#include "Component.h"

#define MAX_LIGHTS 64

// The struct that should match the light data in the shaders.
struct GPU_LIGHT_DATA
{
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 direction;
	float brightness;
	DirectX::XMFLOAT3 position;
	float specularity;
	float radius;
	float spotAngle;
	bool enabled;
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
};

class LightComponent : public Component
{
public:
	LightComponent(Entity& entity);
	~LightComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void lateUpdate(float deltaTime, float totalTime) override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	LightType getLightType() const;

	// Changes the light type to either a point light, directional light, or spot light.
	void setLightType(LightType lightType);

	LightSettings getLightSettings() const;
	void setLightSettings(const LightSettings& settings);

	// Sets the light's settings back to default.
	void useDefaultSettings();

	DirectX::XMFLOAT4X4 getViewMatrix() const;
	DirectX::XMFLOAT4X4 getProjectionMatrix() const;

	bool castShadows;

private:
	void setSettingsDefault();
	void updateViewMatrix();
	void updateProjectionMatrix(float nearZ, float farZ, float width = 100.0f, float height = 100.0f);

	LightSettings m_light;
	LightType m_lightType;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	
	static TwStructMember d_lightStructMembers[5];
	static TwType TW_TYPE_LIGHT_SETTINGS;

	static TwEnumVal d_lightTypeMembers[3];
	static TwType TW_TYPE_LIGHT_TYPE;
};