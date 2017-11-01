#include "../Scene/SceneManager.h"

void TW_CALL copyStringToClient(std::string& destination, const std::string& source)
{
	destination = source;
}



#pragma region Entity
void TW_CALL getEntityEnabledDebugEditor(void* value, void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	*static_cast<bool*>(value) = entity->getEnabled();
}

void TW_CALL setEntityEnabledDebugEditor(const void* value, void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->setEnabled(*static_cast<const bool*>(value));
}

void TW_CALL addChildEntityDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	Entity* child = entity->getScene().getEntityByName(entity->d_childNameInputField);
	if (child)
	{
		entity->addChild(child);
	}
}

void TW_CALL removeChildEntityDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	Entity* child = entity->getScene().getEntityByName(entity->d_childNameInputField);
	if (child)
	{
		entity->removeChild(child);
	}
}

void TW_CALL addEntityDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (!scene->d_entityNameField.empty())
		scene->createEntity(scene->d_entityNameField);
	else
		Debug::warning("Could not create entity because no name was given. Please entity a unique name for this entity.");
}

void TW_CALL removeEntityDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->getScene().deleteEntity(entity);
}

void TW_CALL addComponentDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->addComponentByStringType(entity->d_componentTypeField);
}

void TW_CALL removeComponentDebugEditor(void* clientData)
{
	Component* component = static_cast<Component*>(clientData);
	component->getEntity().removeComponent(component);
}
#pragma endregion



#pragma region Asset
void TW_CALL addTextureDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<Texture>(scene->d_textureNameField, scene->d_texturePathField);
}

void TW_CALL addMaterialDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<Material>(scene->d_materialNameField, scene->d_materialPathField);
}

void TW_CALL addMeshDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<Mesh>(scene->d_meshNameField, scene->d_meshPathField);
}

void TW_CALL addFontDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<Font>(scene->d_fontNameField, scene->d_fontPathField);
}

void TW_CALL addSamplerDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<Sampler>(scene->d_samplerNameField, scene->d_samplerPathField);
}

void TW_CALL addVertexShaderDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<VertexShader>(scene->d_vertexShaderNameField, scene->d_vertexShaderPathField);
}

void TW_CALL addPixelShaderDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	AssetManager::loadAsset<PixelShader>(scene->d_pixelShaderNameField, scene->d_pixelShaderPathField);
}

void TW_CALL removeTextureDebugEditor(void* clientData)
{
	Texture* texture = static_cast<Texture*>(clientData);
	AssetManager::unloadAsset<Texture>(texture->getAssetID());
}

void TW_CALL removeMaterialDebugEditor(void* clientData)
{
	Material* material = static_cast<Material*>(clientData);
	AssetManager::unloadAsset<Material>(material->getAssetID());
}

void TW_CALL removeMeshDebugEditor(void* clientData)
{
	Mesh* mesh = static_cast<Mesh*>(clientData);
	AssetManager::unloadAsset<Mesh>(mesh->getAssetID());
}

void TW_CALL removeFontDebugEditor(void* clientData)
{
	Font* font = static_cast<Font*>(clientData);
	AssetManager::unloadAsset<Font>(font->getAssetID());
}

void TW_CALL removeSamplerDebugEditor(void* clientData)
{
	Sampler* sampler = static_cast<Sampler*>(clientData);
	AssetManager::unloadAsset<Sampler>(sampler->getAssetID());
}

void TW_CALL removeVertexShaderDebugEditor(void* clientData)
{
	VertexShader* vertexShader = static_cast<VertexShader*>(clientData);
	AssetManager::unloadAsset<VertexShader>(vertexShader->getAssetID());
}

void TW_CALL removePixelShaderDebugEditor(void* clientData)
{
	PixelShader* pixelShader = static_cast<PixelShader*>(clientData);
	AssetManager::unloadAsset<PixelShader>(pixelShader->getAssetID());
}
#pragma endregion



#pragma region Scene
void TW_CALL newSceneDebugEditor(void* clientData)
{
	SceneManager::clearActiveScene();
}

void TW_CALL loadSceneDebugEditor(void* clientData)
{
	if (SceneManager::d_sceneNameField.empty())
	{
		Debug::warning("No scene name given. Enter a scene to load.");
		return;
	}

	SceneManager::loadScene(SceneManager::d_sceneNameField);
}

void TW_CALL saveSceneDebugEditor(void* clientData)
{
	SceneManager::saveActiveScene();
}

void TW_CALL getMainCameraSceneDebugEditor(void* value, void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	std::string* mainCameraName = static_cast<std::string*>(value);
	TwCopyStdStringToLibrary(*mainCameraName, scene->getMainCamera()->getEntity().getName());
}

void TW_CALL setMainCameraSceneDebugEditor(const void* value, void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	std::string cameraName = *static_cast<const std::string*>(value);

	Entity* cameraEntity = scene->getEntityByName(cameraName);
	if (!cameraEntity)
	{
		Debug::warning("Failed to set main camera because entity with name " + cameraName + " could not be found.");
		return;
	}

	scene->setMainCamera(cameraEntity);
}
#pragma endregion



