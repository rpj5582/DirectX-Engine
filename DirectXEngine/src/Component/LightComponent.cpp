#include "LightComponent.h"

#include "Transform.h"
#include "../Scene/Scene.h"

#include "GUIDebugSpriteComponent.h"


using namespace DirectX;

LightComponent::LightComponent(Entity& entity) : Component(entity)
{
	m_shadowMap = nullptr;
	m_shadowMapSize = 0;

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());

	m_castShadows = false;

	setLightType(DIRECTIONAL_LIGHT);
	setShadowType(SHADOWTYPE_HARD);
	setSettingsDefault();
}

LightComponent::~LightComponent()
{
	if (!entity.getComponent<LightComponent>())
	{
		if (entity.hasTag(TAG_LIGHT))
			entity.removeTag(TAG_LIGHT);
	}

	if (m_shadowMap)
	{
		deleteShadowMap();
	}
}

void LightComponent::init()
{
	Component::init();

	if (!entity.hasTag(TAG_LIGHT))
		entity.addTag(TAG_LIGHT);

#if defined(DEBUG) || defined(_DEBUG)
	DebugEntity* debugIcon = entity.getDebugIcon();
	if (debugIcon)
	{
		GUIDebugSpriteComponent* debugIconSprite = debugIcon->getGUIDebugSpriteComponent();
		if (debugIconSprite->getTexture()->getAssetID() == DEBUG_TEXTURE_DEFAULTICON)
		{
			debugIconSprite->setTexture(AssetManager::getAsset<Texture>(DEBUG_TEXTURE_LIGHTICON));
		}
	}
#endif
}

void LightComponent::initDebugVariables()
{
	Component::initDebugVariables();

	std::vector<DebugComponentData> lightSettingsStructMembers = 
	{
		{ "Color", D_COLOR, nullptr, nullptr, nullptr, 0, sizeof(XMFLOAT4) },
		{ "Brightness", D_FLOAT, nullptr, nullptr, nullptr, 0, sizeof(float) },
		{ "Specularity", D_FLOAT, nullptr, nullptr, nullptr, 0, sizeof(float) },
		{ "Radius", D_FLOAT, nullptr, nullptr, nullptr, 0, sizeof(float) },
		{ "Spotlight Angle", D_FLOAT, nullptr, nullptr, nullptr, 0, sizeof(float) }
	};

	unsigned int lightSettingsHash = Debug::registerStruct<LightSettings>(lightSettingsStructMembers);
	debugAddStruct("Light Settings", &m_light, lightSettingsHash, 0, nullptr, &debugLightComponentSetLightSettings);

	unsigned int lightTypeHash = Debug::registerEnum<LightType>("Point Light\0Directional Light\0Spotlight\0\0");
	debugAddEnum("Light Type", &m_lightType, lightTypeHash, nullptr, &debugLightComponentSetLightType);

	debugAddUInt("Shadow Map Size", &m_shadowMapSize, nullptr, &debugLightComponentSetShadowMapSize);
	debugAddBool("Cast Shadows", &m_castShadows, nullptr, &debugLightComponentSetCastShadows);

	unsigned int shadowTypeHash = Debug::registerEnum<ShadowType>("Hard Shadows\0Soft Shadows\0Very Soft Shadows\0\0");
	debugAddEnum("Shadow Type", &m_shadowType, shadowTypeHash);
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

		switch (Util::stringHash(lightTypeString.c_str()))
		{
		case Util::stringHash("point"):
			setLightType(POINT_LIGHT);
			break;

		case Util::stringHash("directional"):
			setLightType(DIRECTIONAL_LIGHT);
			break;

		case Util::stringHash("spot"):
			setLightType(SPOT_LIGHT);
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

	LightSettings settings = getLightSettings();

	if (color != dataObject.MemberEnd())
	{
		settings.color = XMFLOAT4(color->value["r"].GetFloat(), color->value["g"].GetFloat(), color->value["b"].GetFloat(), color->value["a"].GetFloat());
	}

	if (brightness != dataObject.MemberEnd())
	{
		settings.brightness = brightness->value.GetFloat();
	}

	if (specularity != dataObject.MemberEnd())
	{
		settings.specularity = specularity->value.GetFloat();
	}

	if (radius != dataObject.MemberEnd())
	{
		settings.radius = radius->value.GetFloat();
	}

	if (spotAngle != dataObject.MemberEnd())
	{
		settings.spotAngle = spotAngle->value.GetFloat();
	}

	setLightSettings(settings);

	rapidjson::Value::MemberIterator shadowMapSize = dataObject.FindMember("shadowMapSize");
	if (shadowMapSize != dataObject.MemberEnd())
	{
		setShadowMapSize(shadowMapSize->value.GetUint());
	}

	rapidjson::Value::MemberIterator castShadows = dataObject.FindMember("castShadows");
	if (castShadows != dataObject.MemberEnd())
	{
		canCastShadows(castShadows->value.GetBool());
	}

	rapidjson::Value::MemberIterator shadowType = dataObject.FindMember("shadowType");
	if (shadowType != dataObject.MemberEnd())
	{
		setShadowType((ShadowType)shadowType->value.GetInt());
	}
}

void LightComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
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
	writer.Bool(m_castShadows);

	writer.Key("shadowMapSize");
	writer.Uint(m_shadowMapSize);

	writer.Key("shadowType");
	writer.Int(m_shadowType);
}

LightType LightComponent::getLightType() const
{
	return m_lightType;
}

void LightComponent::setLightType(LightType lightType)
{
	if (lightType == POINT_LIGHT && m_castShadows)
	{
		Debug::warning("Point light shadow mapping is not yet implemented, disabling shadows for this light.");
		canCastShadows(false);
	}

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
	m_light.radius = max(m_light.radius, 0.001f);
	m_light.spotAngle = max(m_light.spotAngle, 0.001f);
	m_light.spotAngle = min(m_light.spotAngle, 180.0f);

	if(m_lightType != DIRECTIONAL_LIGHT)
		updateProjectionMatrix(NEAR_Z, FAR_Z);
}

void LightComponent::useDefaultSettings()
{
	setSettingsDefault();
}

Texture* LightComponent::getShadowMap() const
{
	return m_shadowMap;
}

DirectX::XMFLOAT4X4 LightComponent::getViewMatrix() const
{
	return m_viewMatrix;
}

DirectX::XMFLOAT4X4 LightComponent::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

unsigned int LightComponent::getShadowMapSize() const
{
	return m_shadowMapSize;
}

void LightComponent::setShadowMapSize(unsigned int powOfTwo)
{
	if ((powOfTwo & (powOfTwo - 1)) != 0)
	{
		Debug::warning("Shadow map size not set for entity " + entity.getName() + " because the size was not a power of two.");
		return;
	}

	m_shadowMapSize = powOfTwo;

	if(m_castShadows)
		createShadowMap();
}

bool LightComponent::canCastShadows() const
{
	return m_castShadows;
}

void LightComponent::canCastShadows(bool castShadows)
{
	if (castShadows && m_lightType == POINT_LIGHT)
	{
		Debug::warning("Point light shadow mapping is not yet implemented.");
		return;
	}

	m_castShadows = castShadows;

	if (m_castShadows)
	{
		if (!createShadowMap())
			m_castShadows = !m_castShadows;
	}
	else
		deleteShadowMap();
}

ShadowType LightComponent::getShadowType() const
{
	return m_shadowType;
}

void LightComponent::setShadowType(ShadowType type)
{
	m_shadowType = type;
}

void LightComponent::setSettingsDefault()
{
	LightSettings settings;

	settings.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	settings.brightness = 1.0f;
	settings.specularity = 32.0f;
	settings.radius = 1.0f;
	settings.spotAngle = 15.0f;

	setLightSettings(settings);
}

bool LightComponent::createShadowMap()
{
	deleteShadowMap(); // In case there is already a shadow map

	TextureParameters parameters = {};
	parameters.usage = D3D11_USAGE_DEFAULT;
	parameters.bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	parameters.textureFormat = DXGI_FORMAT_R32_TYPELESS;
	parameters.shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
	parameters.depthStencilViewFormat = DXGI_FORMAT_D32_FLOAT;
	m_shadowMap = AssetManager::createAsset<Texture>(" tex_" + entity.getName() + "_shadowMap", m_shadowMapSize, m_shadowMapSize, parameters);

	if (!m_shadowMap)
	{
		Debug::error("Failed to create shadow map for light entity " + entity.getName() + ".");
		return false;
	}

	return true;
}

void LightComponent::deleteShadowMap()
{
	if (m_shadowMap)
	{
		AssetManager::unloadAsset<Texture>(m_shadowMap);
		m_shadowMap = nullptr;
	}	
}

void LightComponent::updateViewMatrix()
{
	Transform* transform = entity.getComponent<Transform>();
	if (!transform) return;

	switch (m_lightType)
	{
	case POINT_LIGHT:
	{
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
	case POINT_LIGHT:
	{
		XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
		break;
	}

	case SPOT_LIGHT:
	{
		XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_light.spotAngle * 2.0f), 1.0f, nearZ, m_light.radius);
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

void debugLightComponentSetLightSettings(Component* component, const void* value)
{
	const LightSettings settings = *static_cast<const LightSettings*>(value);
	static_cast<LightComponent*>(component)->setLightSettings(settings);
}

void debugLightComponentSetLightType(Component* component, const void* value)
{
	const LightType type = *static_cast<const LightType*>(value);
	static_cast<LightComponent*>(component)->setLightType(type);
}

void debugLightComponentSetShadowMapSize(Component* component, const void* value)
{
	const unsigned int shadowMapSize = *static_cast<const unsigned int*>(value);
	static_cast<LightComponent*>(component)->setShadowMapSize(shadowMapSize);
}

void debugLightComponentSetCastShadows(Component* component, const void* value)
{
	const bool castShadows = *static_cast<const bool*>(value);
	static_cast<LightComponent*>(component)->canCastShadows(castShadows);
}
