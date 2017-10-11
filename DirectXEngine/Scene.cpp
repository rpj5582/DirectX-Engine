#include "Scene.h"

#include "rapidjson\document.h"
#include "rapidjson\error\en.h"
#include <fstream>
#include <string>

using namespace DirectX;

// Function taken from here: http://rextester.com/YJB48513
static constexpr unsigned int fnv1aBasis = 0x811C9DC5;
static constexpr unsigned int fnv1aPrime = 0x01000193;

constexpr unsigned int stringHash(const char* string, unsigned int h = fnv1aBasis)
{
	return !*string ? h : stringHash(string + 1, static_cast<unsigned int>((h ^ *string) * static_cast<unsigned long long>(fnv1aPrime)));
}

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

	m_entities = std::vector<Entity>();
	m_components = std::unordered_map<Entity, std::vector<Component*>>();
	m_entityCounter = 0;

	m_lights = std::vector<LightComponent*>();
	m_cameras = std::vector<Camera*>();
	m_guis = std::vector<GUIComponent*>();

	m_mainCamera = nullptr;
}

Scene::~Scene()
{
	m_lights.clear();
	m_cameras.clear();
	m_guis.clear();

	while (m_entities.size() > 0)
	{
		deleteEntity(m_entities.back());
	}

	if (m_rasterizerState) m_rasterizerState->Release();

	delete m_guiRenderer;
	delete m_renderer;
}

bool Scene::init()
{
	m_renderer = new Renderer(m_context);
	if (!m_renderer->init()) return false;

	m_guiRenderer = new GUIRenderer(m_device, m_context);
	if (!m_guiRenderer->init()) return false;

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

	if (!loadFromJSON("scene1.json")) return false;

	return true;
}

void Scene::update(float deltaTime, float totalTime)
{
	// Update all entities in the scene
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		std::vector<Component*>* components = getComponentsOfEntity(m_entities[i]);
		for (unsigned int j = 0; j < components->size(); j++)
		{
			if(components->at(j)->enabled)
				components->at(j)->update(deltaTime, totalTime);
		}
	}

	// Preprocess each camera by updating it's view matrix
	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		if(m_cameras[i]->enabled)
			m_cameras[i]->updateViewMatrix();
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

		Entity e = createEntity();

		rapidjson::Value& entityName = entity["name"];
		rapidjson::Value& components = entity["components"];

		for (rapidjson::SizeType j = 0; j < components.Size(); j++)
		{
			rapidjson::Value& component = components[j];

			rapidjson::Value& componentType = component["type"];
			rapidjson::Value& dataArray = component["data"];
			
			switch(stringHash(componentType.GetString()))
			{
			case stringHash("Transform"):
			{
				XMFLOAT3 position = XMFLOAT3();
				XMFLOAT3 rotation = XMFLOAT3();
				XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

				Transform* transform = addComponentToEntity<Transform>(e);

				for (rapidjson::SizeType k = 0; k < dataArray.Size(); k++)
				{
					rapidjson::Value& data = dataArray[k];

					rapidjson::Value::MemberIterator positionIter = data.FindMember("position");
					rapidjson::Value::MemberIterator rotationIter = data.FindMember("rotation");
					rapidjson::Value::MemberIterator scaleIter = data.FindMember("scale");

					if (positionIter != data.MemberEnd())
					{
						position = XMFLOAT3(positionIter->value["x"].GetFloat(), positionIter->value["y"].GetFloat(), positionIter->value["z"].GetFloat());
					}

					if (rotationIter != data.MemberEnd())
					{
						rotation = XMFLOAT3(rotationIter->value["x"].GetFloat(), rotationIter->value["y"].GetFloat(), rotationIter->value["z"].GetFloat());
					}

					if (scaleIter != data.MemberEnd())
					{
						scale = XMFLOAT3(scaleIter->value["x"].GetFloat(), scaleIter->value["y"].GetFloat(), scaleIter->value["z"].GetFloat());
					}
				}

				transform->setPosition(position);
				transform->setRotation(rotation);
				transform->setScale(scale);

				break;
			}

			case stringHash("MeshRenderComponent"):
			{
				std::string materialString = "default";
				std::string meshString = "";
				std::string renderStyleString = "solid";

				MeshRenderComponent* meshRenderComponent = addComponentToEntity<MeshRenderComponent>(e);

				for (rapidjson::SizeType k = 0; k < dataArray.Size(); k++)
				{
					rapidjson::Value& data = dataArray[k];	

					rapidjson::Value::MemberIterator materialIter = data.FindMember("material");
					rapidjson::Value::MemberIterator meshIter = data.FindMember("mesh");
					rapidjson::Value::MemberIterator renderStyleIter = data.FindMember("renderStyle");

					if (materialIter != data.MemberEnd())
					{
						materialString = materialIter->value.GetString();
					}

					if (meshIter != data.MemberEnd())
					{
						meshString = meshIter->value.GetString();
					}

					if (renderStyleIter != data.MemberEnd())
					{
						renderStyleString = renderStyleIter->value.GetString();
					}
				}

				Material* material = AssetManager::getMaterial(materialString);
				Mesh* mesh = AssetManager::getMesh(meshString);
				RenderStyle renderStyle = SOLID;
				switch (stringHash(renderStyleString.c_str()))
				{
				case stringHash("solid"):
					break;

				case stringHash("wireframe"):
					renderStyle = WIREFRAME;
					break;

				case stringHash("solid-wireframe"):
					renderStyle = SOLID_WIREFRAME;
					break;

				default:
					Output::Warning("Invalid render style for component " + std::string(componentType.GetString()) + " on entity " + std::string(entityName.GetString()) + ", using SOLID.");
					break;
				}

				meshRenderComponent->setMaterial(material);
				meshRenderComponent->setMesh(mesh);
				meshRenderComponent->setRenderStyle(renderStyle);

				break;
			}

			case stringHash("Camera"):
			{
				addComponentToEntity<Camera>(e);
				break;
			}

			case stringHash("LightComponent"):
			{
				addComponentToEntity<LightComponent>(e);
				break;
			}

			case stringHash("FreeCamControls"):
			{
				addComponentToEntity<FreeCamControls>(e);
				break;
			}

			default:
				Output::Warning("Invalid component type " + std::string(componentType.GetString()) + ", skipping.");
				break;
			}
		}
	}

	rapidjson::Value& mainCamera = dom["mainCamera"];

	// --------HACK---------
	m_mainCamera = getComponentOfEntity<Camera>(2);
	// --------HACK---------

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

Camera* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(Camera* camera)
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

void Scene::subscribeMouseDown(Component* component)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		if (m_mouseDownCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseDownCallbacks.push_back(component);
}

void Scene::unsubscribeMouseDown(Component* component)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		if (m_mouseDownCallbacks[i] == component)
		{
			m_mouseDownCallbacks.erase(m_mouseDownCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseUp(Component* component)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		if (m_mouseUpCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseUpCallbacks.push_back(component);
}

void Scene::unsubscribeMouseUp(Component* component)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		if (m_mouseUpCallbacks[i] == component)
		{
			m_mouseUpCallbacks.erase(m_mouseUpCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseMove(Component* component)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		if (m_mouseMoveCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseMoveCallbacks.push_back(component);
}

void Scene::unsubscribeMouseMove(Component* component)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		if (m_mouseMoveCallbacks[i] == component)
		{
			m_mouseMoveCallbacks.erase(m_mouseMoveCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseWheel(Component* component)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		if (m_mouseWheelCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseWheelCallbacks.push_back(component);
}

void Scene::unsubscribeMouseWheel(Component* component)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		if (m_mouseWheelCallbacks[i] == component)
		{
			m_mouseWheelCallbacks.erase(m_mouseWheelCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::onMouseDown(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		m_mouseDownCallbacks[i]->onMouseDown(buttonState, x, y);
	}
}

void Scene::onMouseUp(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		m_mouseUpCallbacks[i]->onMouseUp(buttonState, x, y);
	}
}

void Scene::onMouseMove(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		m_mouseMoveCallbacks[i]->onMouseMove(buttonState, x, y);
	}
}

void Scene::onMouseWheel(float wheelDelta, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		m_mouseWheelCallbacks[i]->onMouseWheel(wheelDelta, x, y);
	}
}

void Scene::renderGeometry(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	// Preprocess each light entity to get it's position and direction (for forward rendering).
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>();

	// Only loop through the lights if there is a main camera, since we need its view matrix.
	if (m_mainCamera)
	{
		for (unsigned int i = 0; i < m_lights.size(); i++)
		{
			//  Don't use disabled components
			if (!m_lights[i]->enabled) continue;

			const LightSettings lightSettings = m_lights[i]->getLightSettings();

			// Get position, direction, and type of each light
			Transform* lightTransform = getComponentOfEntity<Transform>(m_lights[i]->getEntity());
			if (lightTransform)
			{
				XMFLOAT3 lightPosition = lightTransform->getPosition();
				XMFLOAT3 lightDirection = lightTransform->getForward();

				unsigned int lightType = (unsigned int)m_lights[i]->getLightType();

				// Creates the final memory-aligned struct that is sent to the GPU
				lightData.push_back(
				{
					lightSettings.color,
					lightDirection,
					lightSettings.brightness,
					lightPosition,
					lightSettings.specularity,
					lightSettings.radius,
					lightSettings.spotAngle,
					m_lights[i]->enabled,
					lightType,
				});
			}
		}
	}

	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	if (lightData.size() > 0)
		m_renderer->render(this, &lightData[0], lightData.size());
	else
		m_renderer->render(this, nullptr, 0);
}

void Scene::renderGUI(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	m_guiRenderer->begin(blendState, depthStencilState);

	if (m_guis.size() > 0)
	{
		m_guiRenderer->render(this, &m_guis[0], m_guis.size());
	}

	m_guiRenderer->end();
}

Entity Scene::createEntity()
{
	Entity entity = m_entityCounter++;

	m_entities.push_back(entity);
	m_components[entity] = std::vector<Component*>();

	return entity;
}

void Scene::deleteEntity(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			m_entities.erase(m_entities.begin() + i);

			for (unsigned int j = 0; j < m_components.size(); j++)
			{
				for (unsigned int k = 0; k < m_components[entity].size(); k++)
				{
					delete m_components[entity][k];
				}
				m_components.erase(entity);
			}
			break;
		}
	}
}

void Scene::getAllEntities(Entity** entities, unsigned int* entityCount)
{
	*entityCount = m_entities.size();

	if (*entityCount > 0)
		*entities = &m_entities[0];
	else
		*entities = nullptr;
}

std::vector<Component*>* Scene::getComponentsOfEntity(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			return &m_components.at(entity);
		}
	}

	return nullptr;
}
