#include "Scene.h"

#include "../Component/FreeCamControls.h"
#include "../Component/GUIDebugSpriteComponent.h"

#include "rapidjson/error/en.h"
#include <fstream>
#include <string>

using namespace DirectX;

Scene::Scene()
{
	m_filepath = "";	

	m_entityCount = 0;
	m_entities = std::vector<Entity*>();
	m_taggedEntities = std::unordered_map<std::string, std::vector<Entity*>>();

	m_debugCamera = nullptr;
	m_mainCamera = nullptr;

	m_dirty = false;
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

	AssetManager::unloadAllAssets();
}

bool Scene::init()
{
	HRESULT hr = S_OK;

	addTag(TAG_MAIN_CAMERA);
	addTag(TAG_LIGHT);
	addTag(TAG_GUI);

	m_debugCamera = new Entity(*this, 0, "DebugCamera", false);
	Transform* debugCameraTransform = m_debugCamera->addComponent<Transform>();
	debugCameraTransform->move(XMFLOAT3(0, 10, -10));
	debugCameraTransform->rotateLocalX(30);
	m_debugCamera->addComponent<CameraComponent>();
	m_debugCamera->addComponent<FreeCamControls>();

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

		// Collision detection
		for (unsigned int i = 0; i < m_entities.size(); i++)
		{
			ICollider* firstCollider = m_entities[i]->getComponent<ICollider>();
			if (firstCollider && firstCollider->enabled)
			{
				for (unsigned int j = 0; j < m_entities.size(); j++)
				{
					ICollider* secondCollider = m_entities[j]->getComponent<ICollider>();
					if (secondCollider && secondCollider->enabled)
					{
						if (firstCollider != secondCollider)
						{
							XMFLOAT3 mtv = firstCollider->doSAT(*secondCollider);
							if (mtv.x != 0 || mtv.y != 0 || mtv.z != 0)
							{
								Debug::message("MTV X: " + std::to_string(mtv.x) + ", Y: " + std::to_string(mtv.y) + ", Z: " + std::to_string(mtv.z));
							}
						}
					}
				}
			}
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

bool Scene::isDirty() const
{
	return m_dirty;
}

bool Scene::hasFilePath() const
{
	return m_filepath != "";
}

bool Scene::loadFromJSON(std::string filepath)
{
	// Load the json file
	std::ifstream ifs(filepath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::warning("Failed to load scene at " + filepath + ": " + errString);
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

		for (rapidjson::SizeType j = 0; j < tags.Size(); j++)
		{
			e->addTag(tags[j].GetString());
		}
	}

	for(auto it = childrenNames.begin(); it != childrenNames.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			it->first->addChildByName(it->second[i]);
		}
	}

	Entity* mainCamera = getEntityWithTag(TAG_MAIN_CAMERA);
	if (mainCamera)
	{
		CameraComponent* mainCameraComponent = mainCamera->getComponent<CameraComponent>();
		if (mainCameraComponent)
			m_mainCamera = mainCameraComponent;
		else
			Debug::warning("Main camera doesn't have a camera component attached for entity " + mainCamera->getName() + ".");
	}
	else
		Debug::warning("No main camera found - tag a camera as the main camera.");
	
	m_filepath = filepath;
	Debug::message("Loaded scene from " + m_filepath);

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->onSceneLoaded();
	}

	return true;
}

void Scene::saveToJSON()
{
	saveToJSON(m_filepath);
}

void Scene::saveToJSON(std::string filepath)
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

	writer.StartObject();

	// Save 2 major components: The dependent assets and the entities.

	// 1. Assets
	writer.Key("assets");
	writer.StartArray();
	AssetManager::saveToJSON(writer);
	writer.EndArray();

	// 2. Entities
	writer.Key("entities");
	writer.StartArray();
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->saveToJSON(writer);
	}
	writer.EndArray();

	writer.EndObject();

	// Now stringify the DOM and save it to a file
	std::ofstream ofs(filepath, std::ios::out);

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

	m_filepath = filepath;
	m_dirty = false;

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

std::vector<std::string> Scene::getAllTags() const
{
	std::vector<std::string> tags = std::vector<std::string>(m_taggedEntities.size());

	unsigned int i = 0;
	for (auto it = m_taggedEntities.begin(); it != m_taggedEntities.end(); it++)
	{
		tags[i] = it->first;
		i++;
	}

	return tags;
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

	if (tag == TAG_MAIN_CAMERA && !m_mainCamera)
		setMainCamera(&entity);
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

			if (tag == TAG_MAIN_CAMERA && m_mainCamera)
				setMainCamera((Entity*)nullptr);

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

	Debug::message("Set main camera to entity " + camera->getEntity().getName());
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
		Debug::warning("Failed to set main camera because entity " + entity->getName() + " does not have a camera component.");
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

void Scene::renderGeometry(Renderer* renderer, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV, float width, float height)
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
					renderer->prepareShadowMapPass(shadowMap);
					renderer->renderShadowMapPass(&m_entities[0], m_entities.size(), *lightComponent);

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

	renderer->prepareMainPass(backBufferRTV, backBufferDSV, width, height);

	if(m_entities.size() > 0)
		renderer->renderMainPass(*camera, Window::getProjectionMatrix(), &m_entities[0], m_entities.size(), &lightData[0], &shadowMatrices[0], &shadowMapSRVs[0]);
}

void Scene::renderGUI(GUIRenderer* guiRenderer)
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

	guiRenderer->begin();

	if (guis.size() > 0)
	{
		guiRenderer->render(&guis[0], guis.size());
	}

	guiRenderer->end();
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
	Entity* entity = new Entity(*this, ++m_entityCount, name, true);
	m_entities.push_back(entity);

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

	delete m_entities[index];
	m_entities.erase(m_entities.begin() + index);
	return;
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
