#include "Scene.h"

#include "../Component/FreeCamControls.h"
#include "../Component/GUIDebugSpriteComponent.h"

#include "rapidjson/error/en.h"
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
	m_taggedEntities = std::unordered_map<std::string, std::vector<Entity*>>();

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
	m_taggedEntities.clear();

	if (m_blendState) m_blendState->Release();
	if (m_depthStencilStateDefault) m_depthStencilStateDefault->Release();
	if (m_depthStencilStateRead) m_depthStencilStateRead->Release();

	delete m_guiRenderer;
	delete m_renderer;
}

bool Scene::init()
{
	HRESULT hr = S_OK;

	m_renderer = new Renderer(m_device, m_context);
	if (!m_renderer->init()) return false;

	m_guiRenderer = new GUIRenderer(m_context);

	addTag(TAG_LIGHT);
	addTag(TAG_GUI);

	m_debugCamera = new Entity(*this, "DebugCamera", false);
	Transform* debugCameraTransform = m_debugCamera->addComponent<Transform>(false);
	debugCameraTransform->move(XMFLOAT3(0, 10, -10));
	debugCameraTransform->rotateLocalX(30);
	m_debugCamera->addComponent<CameraComponent>(false);
	m_debugCamera->addComponent<FreeCamControls>(false);

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
			DebugEntity* entityDebugIcon = m_entities[i]->getDebugIcon();
			if (entityDebugIcon && entityTransform)
			{
				GUIDebugSpriteComponent* debugIconSpriteComponent = entityDebugIcon->getGUIDebugSpriteComponent();
				debugIconSpriteComponent->update(deltaTime, totalTime);
				debugIconSpriteComponent->calculatePosition(entityTransform->getPosition());
			}

			// Run lateUpdate for light components regardless if they're enabled since they need to update in debug mode.
			LightComponent* lightComponent = m_entities[i]->getComponent<LightComponent>();
			if (lightComponent)
			{
				lightComponent->lateUpdate(deltaTime, totalTime);
			}
		}
#endif
	}
}

void Scene::render(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV)
{
	m_context->OMSetDepthStencilState(m_depthStencilStateDefault, 0);
	renderGeometry(backBufferRTV, backBufferDSV);

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
		rapidjson::Value& tags = entity["tags"];
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

		for (rapidjson::SizeType j = 0; j < tags.Size(); j++)
		{
			e->addTag(tags[j].GetString());
		}

		for (rapidjson::SizeType j = 0; j < components.Size(); j++)
		{
			rapidjson::Value& component = components[j];

			rapidjson::Value& componentType = component["type"];
			rapidjson::Value& dataObject = component["data"];
			
			std::string test = componentType.GetString();
			Component* c = e->addComponentByStringType(test);
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
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

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

void Scene::addTag(std::string tag)
{
	if (m_taggedEntities.find(tag) != m_taggedEntities.end())
	{
		Debug::warning("Tag " + tag + " not added because the tag already exists.");
		return;
	}

	m_taggedEntities[tag] = std::vector<Entity*>();
}

void Scene::addTagToEntity(Entity& entity, std::string tag)
{
	if (m_taggedEntities.find(tag) == m_taggedEntities.end())
	{
		addTag(tag);
	}

	if (entity.hasTag(tag))
	{
		Debug::warning("Tag " + tag + " not added to entity " + entity.getName() + " because the entity already has this tag.");
		return;
	}

	m_taggedEntities[tag].push_back(&entity);
	entity.addTagNonResursive(tag);
}

void Scene::removeTagFromEntity(Entity& entity, std::string tag)
{
	if (m_taggedEntities.find(tag) == m_taggedEntities.end())
	{
		Debug::warning("Could not remove tag " + tag + " from entity " + entity.getName() + " because the tag doesn't exist.");
		return;
	}

	std::vector<Entity*>& entities = m_taggedEntities.at(tag);
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		if (entities[i] == &entity)
		{
			entities[i]->removeTagNonRecursive(tag);
			entities.erase(entities.begin() + i);
			return;
		}
	}

	Debug::warning("Tag " + tag + " not removed from entity " + entity.getName() + " because the entity does not have this tag.");
}

CameraComponent* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(CameraComponent* camera)
{
	if (!camera)
	{
		if(m_mainCamera)
			Debug::warning("Scene no longer has a main camera.");

		m_mainCamera = camera;
		return;
	}

	Debug::message("Set main camera for scene " + m_name + " to entity " + camera->getEntity().getName());
	m_mainCamera = camera;
}

void Scene::setMainCamera(Entity* entity)
{
	if (!entity)
	{
		setMainCamera((CameraComponent*)nullptr);
		return;
	}

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

void Scene::renderGeometry(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV)
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
	
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(MAX_LIGHTS);

	std::vector<GPU_SHADOW_MATRICES> shadowMatrices = std::vector<GPU_SHADOW_MATRICES>(MAX_SHADOWMAPS);
	std::vector<ID3D11ShaderResourceView*> shadowMapSRVs = std::vector<ID3D11ShaderResourceView*>(MAX_SHADOWMAPS);

	// Preprocess each light entity to get it's position and direction, and see if it should cast shadows.
	std::vector<Entity*> lightEntities = m_taggedEntities.at(TAG_LIGHT);
	for (unsigned int i = 0; i < lightEntities.size() && i < MAX_LIGHTS; i++)
	{
		if (!lightEntities[i]->getEnabled()) continue;

		LightComponent* lightComponent = lightEntities[i]->getComponent<LightComponent>();

		if (!lightComponent || !lightComponent->enabled) continue;

		const LightSettings lightSettings = lightComponent->getLightSettings();

		// Get position, direction, and type of each light
		Transform* lightTransform = lightEntities[i]->getComponent<Transform>();
		if (!lightTransform) continue;

		XMFLOAT3 lightPosition = lightTransform->getPosition();
		XMFLOAT3 lightDirection = lightTransform->getForward();

		int lightType = (int)lightComponent->getLightType();
		int shadowType = (int)lightComponent->getShadowType();

		bool shadowMapEnabled = false;

		if (i < MAX_SHADOWMAPS)
		{
			if (lightComponent->canCastShadows())
			{
				Texture* shadowMap = lightComponent->getShadowMap();
				if (shadowMap)
				{
					m_renderer->prepareShadowMapPass(shadowMap);
					m_renderer->renderShadowMapPass(&m_entities[0], m_entities.size(), *lightComponent);

					XMFLOAT4X4 lightViewT;
					XMFLOAT4X4 lightView = lightComponent->getViewMatrix();
					XMMATRIX lightViewMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightView));
					XMStoreFloat4x4(&lightViewT, lightViewMatrixT);

					XMFLOAT4X4 lightProjT;
					XMFLOAT4X4 lightProj = lightComponent->getProjectionMatrix();
					XMMATRIX lightProjectionMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightProj));
					XMStoreFloat4x4(&lightProjT, lightProjectionMatrixT);

					shadowMatrices[i] =
					{
						lightViewT,
						lightProjT,
					};

					shadowMapSRVs[i] = shadowMap->getSRV();
					shadowMapEnabled = true;
				}
			}
		}

		// Creates the final memory-aligned struct that is sent to the GPU
		lightData[i] =
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
			shadowMapEnabled,
			shadowType
		};
	}

	if (m_entities.size() > 0)
	{
		m_renderer->prepareMainPass(backBufferRTV, backBufferDSV);
		m_renderer->renderMainPass(*camera, Window::getProjectionMatrix(), &m_entities[0], m_entities.size(), &lightData[0], &shadowMatrices[0], &shadowMapSRVs[0]);
	}
}

void Scene::renderGUI()
{
	std::vector<GUIComponent*> guis;
	std::vector<Entity*>& guiEntities = m_taggedEntities.at(TAG_GUI);
	for (unsigned int i = 0; i < guiEntities.size(); i++)
	{
		if (!m_entities[i]->getEnabled()) continue;

		GUIComponent* gui = guiEntities[i]->getComponent<GUIComponent>();
		if (gui && gui->enabled)
		{
			guis.push_back(gui);
		}
	}

#if defined(DEBUG) || defined(_DEBUG)
	if (!Debug::inPlayMode)
	{
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			DebugEntity* debugIcon = m_entities[i]->getDebugIcon();
			if (debugIcon && debugIcon->enabled)
			{
				guis.push_back(debugIcon->getGUIDebugSpriteComponent());
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

unsigned int Scene::getEntityIndex(const Entity& entity) const
{
	unsigned int index = 0;
	for (; index < m_entities.size(); index++)
	{
		if (m_entities[index] == &entity)
		{
			return index;
		}
	}

	Debug::warning("Could not get the index of entity " + entity.getName() + " because it wasn't found in the scene's entity list (How did that happen???).");
	return -1;
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

	unsigned int index = getEntityIndex(*entity);
	if (index < 0)
	{
		Debug::warning("Failed to delete entity " + entity->getName() + ".");
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
				deleteEntity(children[i]);
				break;
			}
		}
	}
	children.clear();

	// Recalculate the index since deleting children may have shifted the entity list
	index = getEntityIndex(*entity);
	if (index < 0)
	{
		Debug::warning("Failed to delete entity " + entity->getName() + ".");
		return;
	}

	// Remove entity from tag lists
	std::unordered_set<std::string> entityTags = m_entities[index]->getTags();
	for (auto it = entityTags.begin(); it != entityTags.end(); it++)
	{
		removeTagFromEntity(*m_entities[index], *it);
	}

	Debug::entityDebugWindow->removeEntity(entity);
	delete m_entities[index];
	m_entities.erase(m_entities.begin() + index);
	return;
}

void Scene::clear()
{
	while (m_entities.size() > 0)
	{
		deleteEntity(m_entities.back());
	}

	setMainCamera((CameraComponent*)nullptr);
	
	Debug::sceneDebugWindow->clear();
	Debug::entityDebugWindow->clear();
	Debug::assetDebugWindow->clear();

	AssetManager::unloadAllAssets();
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

Entity* Scene::getEntityWithTag(std::string tag)
{
	std::vector<Entity*> entities = getAllEntitiesWithTag(tag);
	if (entities.size() == 0)
	{
		Debug::warning("No entities in the scene have a tag " + tag + ".");
		return nullptr;
	}

	return entities[0];
}

std::vector<Entity*> Scene::getAllEntities() const
{
	return m_entities;
}

std::vector<Entity*> Scene::getAllEntitiesWithTag(std::string tag) const
{
	if (m_taggedEntities.find(tag) == m_taggedEntities.end())
	{
		Debug::warning("Tag " + tag + " doesn't exist.");
		return std::vector<Entity*>();
	}

	return m_taggedEntities.at(tag);
}
