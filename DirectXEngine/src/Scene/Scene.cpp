#include "Scene.h"

#include "../rapidjson/error/en.h"
#include <fstream>
#include <string>

using namespace DirectX;

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath)
{
	m_device = device;
	m_context = context;

	m_name = name;
	m_filepath = filepath;

	m_renderer = nullptr;
	m_guiRenderer = nullptr;

	m_blendState = nullptr;
	m_depthStencilStateDefault = nullptr;
	m_depthStencilStateRead = nullptr;

	m_entities = std::vector<Entity*>();

	m_debugCamera = nullptr;
	m_mainCamera = nullptr;

	d_entityNameField = "";
	d_mainCameraField = "";

	d_textureNameField = "";
	d_materialNameField = "";
	d_meshNameField = "";
	d_fontNameField = "";
	d_samplerNameField = "";
	d_vertexShaderNameField = "";
	d_pixelShaderNameField = "";

	d_texturePathField = "";
	d_materialPathField = "";
	d_meshPathField = "";
	d_fontPathField = "";
	d_samplerPathField = "";
	d_vertexShaderPathField = "";
	d_pixelShaderPathField = "";
}

Scene::~Scene()
{
	delete m_debugCamera;

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}
	m_entities.clear();

	if (m_blendState) m_blendState->Release();
	if (m_depthStencilStateDefault) m_depthStencilStateDefault->Release();
	if (m_depthStencilStateRead) m_depthStencilStateRead->Release();

	delete m_guiRenderer;
	delete m_renderer;
}

bool Scene::init()
{
	m_debugCamera = new Entity(*this, "DebugCamera", false);
	Transform* debugCameraTransform = m_debugCamera->addComponent<Transform>(false);
	debugCameraTransform->move(XMFLOAT3(0, 10, -10));
	debugCameraTransform->rotateLocalX(30);
	m_debugCamera->addComponent<CameraComponent>(false);
	m_debugCamera->addComponent<FreeCamControls>(false);

	m_renderer = new Renderer(m_context);
	if (!m_renderer->init()) return false;

	m_guiRenderer = new GUIRenderer(m_context);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_DEPTH_STENCIL_DESC depthStencilDefaultDesc = {};
	depthStencilDefaultDesc.DepthEnable = true;
	depthStencilDefaultDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDefaultDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDefaultDesc.StencilEnable = false;
	depthStencilDefaultDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDefaultDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDefaultDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDefaultDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDefaultDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDefaultDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDefaultDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDefaultDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDefaultDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDefaultDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	D3D11_DEPTH_STENCIL_DESC depthStencilReadDesc = depthStencilDefaultDesc;
	depthStencilReadDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	HRESULT hr = S_OK;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendState);
	if (FAILED(hr))
	{
		Debug::error("Failed to create blend state.");
		return false;
	}

	hr = m_device->CreateDepthStencilState(&depthStencilDefaultDesc, &m_depthStencilStateDefault);
	if (FAILED(hr))
	{
		Debug::error("Failed to create default depth stencil state.");
		return false;
	}

	hr = m_device->CreateDepthStencilState(&depthStencilReadDesc, &m_depthStencilStateRead);
	if (FAILED(hr))
	{
		Debug::error("Failed to create read depth stencil state.");
		return false;
	}

	m_context->OMSetBlendState(m_blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(m_depthStencilStateDefault, 0);

	return true;
}

void Scene::update(float deltaTime, float totalTime)
{
	if (Debug::inPlayMode)
	{
		// Update all entities in the scene
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			if (m_entities[i]->getEnabled())
				m_entities[i]->update(deltaTime, totalTime);
		}

		// LateUpdate for all entities in the scene
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			if (m_entities[i]->getEnabled())
				m_entities[i]->lateUpdate(deltaTime, totalTime);
		}
	}
	else
	{
#if defined(DEBUG) || defined(_DEBUG)
		m_debugCamera->update(deltaTime, totalTime);
		m_debugCamera->lateUpdate(deltaTime, totalTime);

		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			Transform* entityTransform = m_entities[i]->getComponent<Transform>();
			GUIDebugSpriteComponent* debugIconSpriteComponent = m_entities[i]->getDebugIconSpriteComponent();

			if (entityTransform && entityTransform->enabled && debugIconSpriteComponent)
			{
				debugIconSpriteComponent->update(deltaTime, totalTime);
				debugIconSpriteComponent->calculatePosition(entityTransform->getPosition());
			}
		}
#endif
	}
}

void Scene::render()
{
	m_context->OMSetDepthStencilState(m_depthStencilStateDefault, 0);
	renderGeometry();

	m_context->OMSetDepthStencilState(m_depthStencilStateRead, 0);
	renderGUI();
}

std::string Scene::getName() const
{
	return m_name;
}

bool Scene::loadFromJSON()
{
	// Load the json file
	std::ifstream ifs(m_filepath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::warning("Failed to load scene at " + m_filepath + ": " + errString);
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

		Debug::error("Failed to load scene at " + m_filepath + " because there was a parse error at character " +  std::to_string(result.Offset()) + ": " + std::string(errorMessage));
		delete[] jsonStringBuffer;
		return false;
	}

	delete[] jsonStringBuffer;

	ifs.close();

	clear();

	// Load the scene's dependent assets.
	rapidjson::Value& assets = dom["assets"];
	AssetManager::loadFromJSON(assets);

	// Load the scene's entities.
	std::unordered_map<Entity*, std::vector<std::string>> childrenNames;

	rapidjson::Value& entities = dom["entities"];
	for (rapidjson::SizeType i = 0; i < entities.Size(); i++)
	{
		rapidjson::Value& entity = entities[i];

		rapidjson::Value& entityName = entity["name"];
		rapidjson::Value& children = entity["children"];
		rapidjson::Value& components = entity["components"];

		Entity* e = createEntity(entityName.GetString());

		rapidjson::Value::MemberIterator entityEnabled = entity.FindMember("enabled");
		if (entityEnabled != entity.MemberEnd())
		{
			e->setEnabled(entityEnabled->value.GetBool());
		}

		childrenNames[e] = std::vector<std::string>();
		for (rapidjson::SizeType j = 0; j < children.Size(); j++)
		{
			rapidjson::Value& childName = children[j];
			childrenNames.at(e).push_back(childName.GetString());
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
				Debug::warning("Skipping component of type " + std::string(componentType.GetString()) + " - either an invalid type or the component was not registered.");
		}
	}

	for(auto it = childrenNames.begin(); it != childrenNames.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			it->first->addChildByName(it->second[i]);
		}
	}

	// Loads the scene's main camera
	rapidjson::Value& mainCamera = dom["mainCamera"];

	Entity* cameraEntity = getEntityByName(mainCamera.GetString());
	if (cameraEntity)
		setMainCamera(cameraEntity);
	else
		Debug::warning("Could not set main camera because entity with name " + std::string(mainCamera.GetString()) + " does not exist.");
	
	Debug::message("Loaded scene from " + m_filepath);
	onLoad();

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onSceneLoaded();
	}

	return true;
}

void Scene::saveToJSON()
{
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	writer.StartObject();

	// Save 3 major components: The main camera, the dependent assets, and the entities.

	// 1. Camera
	if (m_mainCamera)
	{
		std::string mainCameraName = m_mainCamera->getEntity().getName();
		writer.Key("mainCamera");
		writer.String(mainCameraName.c_str());
	}
	else
	{
		writer.Key("mainCamera");
		writer.String("undefined");
	}

	// 2. Assets
	writer.Key("assets");
	writer.StartArray();
	AssetManager::saveToJSON(writer);
	writer.EndArray();

	// 3. Entities
	writer.Key("entities");
	writer.StartArray();
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->saveToJSON(writer);
	}
	writer.EndArray();

	writer.EndObject();

	// Now stringify the DOM and save it to a file
	std::ofstream ofs(m_filepath, std::ios::out);

	if (!ofs.is_open())
	{
		Debug::error("Failed to create file at " + m_filepath);

		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::error("Failed to create file at " + m_filepath + ": " + errString);
		return;
	}

	ofs.write(s.GetString(), s.GetSize());
	ofs.close();

	Debug::message("Saved scene to " + m_filepath);
}

CameraComponent* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(CameraComponent* camera)
{
	Debug::message("Set main camera for scene " + m_name + " to entity " + camera->getEntity().getName());
	m_mainCamera = camera;
}

void Scene::setMainCamera(Entity* entity)
{
	CameraComponent* camera = entity->getComponent<CameraComponent>();
	if (!camera)
	{
		Debug::warning("Failed to set main camera because entity" + entity->getName() + " does not have a camera component.");
		return;
	}

	setMainCamera(camera);
}

CameraComponent* Scene::getDebugCamera() const
{
#if defined(DEBUG) || defined(_DEBUG)
	return m_debugCamera->getComponent<CameraComponent>();
#endif

	return nullptr;
}

void Scene::renderGeometry()
{
	CameraComponent* camera = nullptr;

	if (Debug::inPlayMode)
	{
		if (m_mainCamera)
			camera = m_mainCamera;
		else
			return;
	}
	else
	{
		camera = m_debugCamera->getComponent<CameraComponent>();
	}
	

	// Preprocess each light entity to get it's position and direction (for forward rendering).
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(MAX_LIGHTS);
	unsigned int lightCount = 0;
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (!m_entities[i]->getEnabled()) continue;

		LightComponent* lightComponent = nullptr;
		std::vector<Component*>& components = m_entities[i]->getAllComponents();
		for (unsigned int j = 0; j < components.size(); j++)
		{
			lightComponent = dynamic_cast<LightComponent*>(components[j]);
			if (lightComponent)
				break;
		}

		if (!lightComponent || !lightComponent->enabled) continue;

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

	if (m_entities.size() > 0)
	{
		m_renderer->render(*camera, Window::getProjectionMatrix(), &m_entities[0], m_entities.size(), &lightData[0]);
	}
}

void Scene::renderGUI()
{
	std::vector<GUIComponent*> guis;

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (!m_entities[i]->getEnabled()) continue;

		GUIComponent* guiComponent = nullptr;
		std::vector<Component*>& components = m_entities[i]->getAllComponents();
		for (unsigned int j = 0; j < components.size(); j++)
		{
			guiComponent = dynamic_cast<GUIComponent*>(components[j]);
			if (guiComponent)
				break;
				
		}

		if (!guiComponent || !guiComponent->enabled) continue;

		guis.push_back(guiComponent);
	}

#if defined(DEBUG) || defined(_DEBUG)
	if (!Debug::inPlayMode)
	{
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			GUITransform* debugIconTransform = m_entities[i]->getDebugIconTransform();
			GUIDebugSpriteComponent* debugIconSpriteComponent = m_entities[i]->getDebugIconSpriteComponent();

			if (debugIconTransform && debugIconTransform->enabled && debugIconSpriteComponent && debugIconSpriteComponent->enabled)
			{
				guis.push_back(debugIconSpriteComponent);
			}
		}
	}
#endif

	m_guiRenderer->begin(m_blendState, m_depthStencilStateRead);

	if (guis.size() > 0)
	{
		m_guiRenderer->render(&guis[0], guis.size());
	}

	m_guiRenderer->end();
}

Entity* Scene::createEntity(std::string name)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i]->getName() == name)
		{
			Debug::warning("Failed to create entity with name " + name + " because an entity with that name already exists.");
			return nullptr;
		}
	}

	Entity* entity = new Entity(*this, name, true);
	m_entities.push_back(entity);

	Debug::entityDebugWindow->addEntity(entity);

	return entity;
}

void Scene::deleteEntity(Entity* entity)
{
	if (!entity)
	{
		Debug::warning("Attempted to delete a null entity, skipping.");
		return;
	}

	unsigned int index = 0;
	for (; index < m_entities.size(); index++)
	{
		if (m_entities[index] == entity)
		{
			break;
		}
	}

	if (index == m_entities.size())
	{
		Debug::warning("Could not delete entity " + entity->getName() + " because it wasn't found in the scene's entity list (How did that happen???).");
		return;
	}

	std::vector<Entity*> children = m_entities[index]->getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		// Find the child entity in the big entity list and remove its pointer
		for (unsigned int j = 0; j < m_entities.size(); j++)
		{
			if (m_entities[j] == children[i])
			{
				m_entities.erase(m_entities.begin() + j);
				delete children[i];
				break;
			}
		}
	}
	children.clear();

	Debug::entityDebugWindow->removeEntity(entity);
	delete m_entities[index];
	m_entities.erase(m_entities.begin() + index);
	return;
}

void Scene::clear()
{
	while (m_entities.size() > 0)
	{
		delete m_entities.back();
		m_entities.pop_back();
	}

	m_mainCamera = nullptr;
	
	AssetManager::unloadAllAssets();

	Debug::entityDebugWindow->clear();
	Debug::assetDebugWindow->clear();
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

	Debug::warning("Failed to find entity with name " + name);
	return nullptr;
}

std::vector<Entity*> Scene::getAllEntities() const
{
	return m_entities;
}
