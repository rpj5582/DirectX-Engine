#include "Scene.h"

#include "rapidjson\error\en.h"
#include <fstream>
#include <string>

using namespace DirectX;

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;

	m_renderer = nullptr;
	m_guiRenderer = nullptr;

	m_rasterizerState = nullptr;
	m_prevUseWireframe = false;

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
	m_near = 0;
	m_far = 0;

	m_entities = std::vector<Entity*>();

	m_mainCamera = nullptr;
}

Scene::~Scene()
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}
	m_entities.clear();

	if (m_rasterizerState) m_rasterizerState->Release();

	delete m_guiRenderer;
	delete m_renderer;
}

bool Scene::init()
{
	m_renderer = new Renderer(m_context);
	if (!m_renderer->init()) return false;

	m_guiRenderer = new GUIRenderer(m_context);

	// Add a rasterizer state so that we can control whether wireframe is on or off.
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.DepthClipEnable = true;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	HRESULT hr = m_device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
	if (FAILED(hr))
	{
		Output::Error("Failed to create rasterizer state.");
		return false;
	}

	m_context->RSSetState(m_rasterizerState);

	return true;
}

void Scene::update(float deltaTime, float totalTime)
{
	// Update all entities in the scene
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i]->enabled)
			m_entities[i]->update(deltaTime, totalTime);
	}

	// LateUpdate for all entities in the scene
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if(m_entities[i]->enabled)
			m_entities[i]->lateUpdate(deltaTime, totalTime);
	}
}

void Scene::render()
{
	CommonStates states(m_device);
	ID3D11BlendState* blendState = states.Opaque();
	ID3D11DepthStencilState* depthStencilState = states.DepthDefault();

	renderGeometry(blendState, depthStencilState);

	blendState = states.NonPremultiplied();
	depthStencilState = states.DepthRead();

	renderGUI(blendState, depthStencilState);
}

bool Scene::loadFromJSON(std::string filepath)
{
	std::string filePath = "Assets/Scenes/" + filepath;

	std::ifstream ifs(filePath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Output::Error("Failed to load scene at " + filePath + ": " + errString);
		return false;
	}

	unsigned int length;
	ifs.seekg(0, std::ios::end);
	length = (unsigned int)ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	char* jsonStringBuffer = new char[length + 1];
	ifs.read(jsonStringBuffer, length);
	jsonStringBuffer[length] = '\0';

	rapidjson::Document dom;
	rapidjson::ParseResult result = dom.Parse(jsonStringBuffer);
	if (result.IsError())
	{
		const char* errorMessage = rapidjson::GetParseError_En(result.Code());

		Output::Error("Failed to load scene at " + filePath + " because there was a parse error at character " +  std::to_string(result.Offset()) + ": " + std::string(errorMessage));
		delete jsonStringBuffer;
		return false;
	}

	delete jsonStringBuffer;

	rapidjson::Value& assets = dom["assets"];
	for (rapidjson::SizeType i = 0; i < assets.Size(); i++)
	{
		rapidjson::Value& asset = assets[i];
		rapidjson::Value& assetType = asset["type"];
		
		switch (stringHash(assetType.GetString()))
		{
		case stringHash("texture"):
		{
			rapidjson::Value& idRef = asset["id"];
			rapidjson::Value& pathRef = asset["path"];

			std::string id = idRef.GetString();
			std::string path = pathRef.GetString();

			AssetManager::loadTexture(id, path);
			break;
		}

		case stringHash("model"):
		{
			rapidjson::Value& idRef = asset["id"];
			rapidjson::Value& pathRef = asset["path"];

			std::string id = idRef.GetString();
			std::string path = pathRef.GetString();

			AssetManager::loadMesh(id, path);
			break;
		}

		case stringHash("font"):
		{
			rapidjson::Value& idRef = asset["id"];
			rapidjson::Value& pathRef = asset["path"];

			std::string id = idRef.GetString();
			std::string path = pathRef.GetString();

			AssetManager::loadFont(id, path);
			break;
		}

		case stringHash("shader"):
		{
			rapidjson::Value& idRef = asset["id"];
			rapidjson::Value& pathRef = asset["path"];
			rapidjson::Value::MemberIterator shaderTypeIter = asset.FindMember("shaderType");

			std::string id = idRef.GetString();
			std::string path = pathRef.GetString();

			std::string shaderTypeString = "default";
			if (shaderTypeIter != asset.MemberEnd())
			{
				shaderTypeString = shaderTypeIter->value.GetString();
			}

			ShaderType shaderType = VertexShader;
			if (shaderTypeString == "pixelShader")
			{
				shaderType = PixelShader;
			}

			AssetManager::loadShader(id, shaderType, path);
			break;
		}

		case stringHash("material"):
		{
			rapidjson::Value& idRef = asset["id"];

			rapidjson::Value::MemberIterator diffuseIter = asset.FindMember("diffuse");
			rapidjson::Value::MemberIterator specularIter = asset.FindMember("specular");
			rapidjson::Value::MemberIterator normalIter = asset.FindMember("normal");
			rapidjson::Value::MemberIterator samplerIter = asset.FindMember("sampler");
			rapidjson::Value::MemberIterator vertexShaderIter = asset.FindMember("vertexShader");
			rapidjson::Value::MemberIterator pixelShaderIter = asset.FindMember("pixelShader");

			std::string id = idRef.GetString();
			std::string diffuse = "defaultDiffuse";
			std::string specular = "defaultSpecular";
			std::string normal = "defaultNormal";
			std::string sampler = "default";
			std::string vertexShader = "default";
			std::string pixelShader = "default";

			if (diffuseIter != asset.MemberEnd())
			{
				diffuse = diffuseIter->value.GetString();
			}

			if (specularIter != asset.MemberEnd())
			{
				specular = specularIter->value.GetString();
			}

			if (normalIter != asset.MemberEnd())
			{
				normal = normalIter->value.GetString();
			}

			if (samplerIter != asset.MemberEnd())
			{
				sampler = samplerIter->value.GetString();
			}

			if (vertexShaderIter != asset.MemberEnd())
			{
				vertexShader = vertexShaderIter->value.GetString();
			}

			if (pixelShaderIter != asset.MemberEnd())
			{
				pixelShader = pixelShaderIter->value.GetString();
			}

			AssetManager::createMaterial(id, diffuse, specular, normal, sampler, vertexShader, pixelShader);
			break;
		}

		case stringHash("sampler"):
		{
			rapidjson::Value& idRef = asset["id"];
			rapidjson::Value::MemberIterator addressUIter = asset.FindMember("addressU");
			rapidjson::Value::MemberIterator addressVIter = asset.FindMember("addressV");
			rapidjson::Value::MemberIterator addressWIter = asset.FindMember("addressW");
			rapidjson::Value::MemberIterator filterIter = asset.FindMember("filter");
			rapidjson::Value::MemberIterator maxLODIter = asset.FindMember("maxLOD");

			std::string id = idRef.GetString();
			std::string addressUString = "wrap";
			std::string addressVString = "wrap";
			std::string addressWString = "wrap";
			std::string filterString = "min_mag_mip_linear";
			std::string maxLODString = "float_max";

			if (addressUIter != asset.MemberEnd())
			{
				addressUString = addressUIter->value.GetString();
			}

			if (addressVIter != asset.MemberEnd())
			{
				addressVString = addressVIter->value.GetString();
			}

			if (addressWIter != asset.MemberEnd())
			{
				addressWString = addressWIter->value.GetString();
			}

			if (filterIter != asset.MemberEnd())
			{
				filterString = filterIter->value.GetString();
			}

			if (maxLODIter != asset.MemberEnd())
			{
				maxLODString = maxLODIter->value.GetString();
			}

			D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			float maxLOD = D3D11_FLOAT32_MAX;

			if (addressUString == "border")
			{
				addressU = D3D11_TEXTURE_ADDRESS_BORDER;
			}
			else if (addressUString == "clamp")
			{
				addressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			}
			else if (addressUString == "mirror")
			{
				addressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			}
			else if (addressUString == "mirror_once")
			{
				addressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
			}

			if (addressVString == "border")
			{
				addressV = D3D11_TEXTURE_ADDRESS_BORDER;
			}
			else if (addressVString == "clamp")
			{
				addressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			}
			else if (addressVString == "mirror")
			{
				addressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			}
			else if (addressVString == "mirror_once")
			{
				addressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
			}

			if (addressWString == "border")
			{
				addressW = D3D11_TEXTURE_ADDRESS_BORDER;
			}
			else if (addressWString == "clamp")
			{
				addressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			}
			else if (addressWString == "mirror")
			{
				addressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			}
			else if (addressWString == "mirror_once")
			{
				addressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
			}

			// Don't support different sampler filters yet because there are too many options

			try
			{
				maxLOD = (float)stoi(maxLODString);
			}
			catch (std::invalid_argument&) {}

			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.AddressU = addressU;
			samplerDesc.AddressV = addressV;
			samplerDesc.AddressW = addressW;
			samplerDesc.Filter = filter;
			samplerDesc.MaxLOD = maxLOD;

			AssetManager::createSampler(id, samplerDesc);
			break;
		}

		default:
			Output::Warning("Invalid asset type " + std::string(assetType.GetString()) + ", skipping.");
			break;
		}
	}

	rapidjson::Value& entities = dom["entities"];
	for (rapidjson::SizeType i = 0; i < entities.Size(); i++)
	{
		rapidjson::Value& entity = entities[i];

		rapidjson::Value& entityName = entity["name"];
		rapidjson::Value& components = entity["components"];

		Entity* e = createEntity(entityName.GetString());

		rapidjson::Value::MemberIterator entityEnabled = entity.FindMember("enabled");
		if (entityEnabled != entity.MemberEnd())
		{
			e->enabled = entityEnabled->value.GetBool();
		}

		for (rapidjson::SizeType j = 0; j < components.Size(); j++)
		{
			rapidjson::Value& component = components[j];

			rapidjson::Value& componentType = component["type"];
			rapidjson::Value& dataObject = component["data"];
			
			Component* c = e->addComponentByStringType(componentType.GetString());
			if (c)
				c->loadFromJSON(dataObject);
			else
				Output::Warning("Skipping component of type " + std::string(componentType.GetString()) + " - either an invalid type or the component was not registered.");
		}
	}

	rapidjson::Value& mainCamera = dom["mainCamera"];

	Entity* cameraEntity = getEntityByName(mainCamera.GetString());
	if (cameraEntity)
	{
		m_mainCamera = cameraEntity->getComponent<CameraComponent>();

		if (!m_mainCamera)
		{
			Output::Warning("Could not set main camera because given entity " + std::string(mainCamera.GetString()) + " does not have a Camera component.");
		}
	}
	else
	{
		Output::Warning("Could not set main camera because entity with name " + std::string(mainCamera.GetString()) + " does not exist.");
	}
	
	return true;
}

DirectX::XMMATRIX Scene::getProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

void Scene::updateProjectionMatrix(int width, int height, float nearZ, float farZ)
{
	m_near = nearZ;
	m_far = farZ;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / height, m_near, m_far);
	XMStoreFloat4x4(&m_projectionMatrix, perspective);
}

float Scene::getNearZ() const
{
	return m_near;
}

float Scene::getFarZ() const
{
	return m_far;
}

CameraComponent* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(CameraComponent* camera)
{
	m_mainCamera = camera;
}

void Scene::drawInWireframeMode(bool wireframe)
{
	if (m_prevUseWireframe == wireframe) return;

	// Get the rasterizer state's current settings
	D3D11_RASTERIZER_DESC rasterizerDesc;
	m_rasterizerState->GetDesc(&rasterizerDesc);

	if (wireframe)
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = -1000;
		rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	}
	else
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	}

	// Recreate the entire rasterizer state
	m_rasterizerState->Release();
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	m_context->RSSetState(m_rasterizerState);

	m_prevUseWireframe = wireframe;
}

void Scene::onMouseDown(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onMouseDown(buttonState, x, y);
	}
}

void Scene::onMouseUp(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onMouseUp(buttonState, x, y);
	}
}

void Scene::onMouseMove(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onMouseMove(buttonState, x, y);
	}
}

void Scene::onMouseWheel(float wheelDelta, int x, int y)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onMouseWheel(wheelDelta, x, y);
	}
}

void Scene::renderGeometry(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	// Preprocess each light entity to get it's position and direction (for forward rendering).
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(MAX_LIGHTS);

	// Only loop through the lights if there is a main camera, since we need its view matrix.
	if (m_mainCamera)
	{
		unsigned int lightCount = 0;
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			//  Don't use disabled entities
			if (!m_entities[i]->enabled) continue;

			LightComponent* lightComponent = nullptr;
			std::vector<Component*>& components = m_entities[i]->getComponents();
			for (unsigned int j = 0; j < components.size(); j++)
			{
				lightComponent = dynamic_cast<LightComponent*>(components[j]);
				if (lightComponent)
					break;
			}

			if (!lightComponent) continue;

			const LightSettings lightSettings = lightComponent->getLightSettings();

			// Get position, direction, and type of each light
			Transform* lightTransform = m_entities[i]->getComponent<Transform>();
			if (!lightTransform) continue;

			XMFLOAT3 lightPosition = lightTransform->getPosition();
			XMFLOAT3 lightDirection = lightTransform->getForward();

			unsigned int lightType = (unsigned int)lightComponent->getLightType();

			// Creates the final memory-aligned struct that is sent to the GPU
			lightCount = (lightCount + 1) % MAX_LIGHTS;
			lightData[lightCount] = 
			{
				lightSettings.color,
				lightDirection,
				lightSettings.brightness,
				lightPosition,
				lightSettings.specularity,
				lightSettings.radius,
				lightSettings.spotAngle,
				lightComponent->enabled,
				lightType,
			};
		}
	}

	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	if (lightData.size() > 0)
		m_renderer->render(this, &lightData[0]);
	else
		m_renderer->render(this, nullptr);
}

void Scene::renderGUI(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	std::vector<GUIComponent*> guis;

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		GUIComponent* guiComponent = nullptr;
		std::vector<Component*>& components = m_entities[i]->getComponents();
		for (unsigned int j = 0; j < components.size(); j++)
		{
			guiComponent = dynamic_cast<GUIComponent*>(components[j]);
			if (guiComponent)
				break;
		}

		if (!guiComponent) continue;

		guis.push_back(guiComponent);
	}

	m_guiRenderer->begin(blendState, depthStencilState);

	if (guis.size() > 0)
	{
		m_guiRenderer->render(&guis[0], guis.size());
	}

	m_guiRenderer->end();
}

Entity* Scene::createEntity(std::string name)
{
	Entity* entity = new Entity(name);
	m_entities.push_back(entity);

	return entity;
}

void Scene::deleteEntity(Entity* entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			delete m_entities[i];
			m_entities.erase(m_entities.begin() + i);

			return;
		}
	}
}

void Scene::getAllEntities(Entity*** entities, unsigned int* entityCount)
{
	*entityCount = m_entities.size();

	if (*entityCount > 0)
		*entities = &m_entities[0];
	else
		*entities = nullptr;
}

Entity* Scene::getEntityByName(std::string name)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i]->getName() == name)
		{
			return m_entities[i];
		}
	}

	Output::Warning("Failed to find entity with name " + name);
	return nullptr;
}
