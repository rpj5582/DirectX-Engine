#pragma once

#include "VertexShader.h"
#include "PixelShader.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Font.h"
#include "Sampler.h"

#include "../Debug/Debug.h"

#include <unordered_map>

#define DEFAULT_SHADER_VERTEX "defaultVertex"
#define DEFAULT_SHADER_PIXEL "defaultPixel"

#define DEFAULT_TEXTURE_DIFFUSE "defaultDiffuse"
#define DEFAULT_TEXTURE_SPECULAR "defaultSpecular"
#define DEFAULT_TEXTURE_NORMAL "defaultNormal"
#define DEFAULT_TEXTURE_GUI "defaultGUI"

#define DEFAULT_MATERIAL "defaultMaterial"

#define DEFAULT_SAMPLER "defaultSampler"

#define DEFAULT_FONT "arial_16pt"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_TEXTURE_DEFAULTICON "debugDefaultIcon"
#define DEBUG_TEXTURE_LIGHTICON "debugLightIcon"
#define DEBUG_TEXTURE_CAMERAICON "debugCameraIcon"
#endif

class AssetManager
{
public:
	AssetManager(ID3D11Device* device, ID3D11DeviceContext* context);
	~AssetManager();

	bool init();

	static void loadFromJSON(rapidjson::Value& assetsArray);
	static void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	template<typename T, typename... Args>
	static T* createAsset(std::string assetID, Args... args);

	template<typename T>
	static T* loadAsset(std::string assetID, std::string filepath);

	template<typename T>
	static T* getAsset(std::string assetID);

	template<typename T>
	static void unloadAsset(std::string assetID);

	static void unloadAllAssets();

	static bool isDefaultAsset(std::string assetName);

private:
	bool loadDefaultAssets();
	void deleteAssets();

	static AssetManager* m_instance;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::unordered_map<std::string, Asset*> m_assets;
};

template<typename T, typename... Args>
inline T* AssetManager::createAsset(std::string assetID, Args... args)
{
	auto it = m_instance->m_assets.find(assetID);
	if (it != m_instance->m_assets.end())
	{
		Debug::warning("Did not create asset with ID " + assetID + " because an asset with that ID already exists.");
		return nullptr;
	}

	T* asset = new T(m_instance->m_device, m_instance->m_context, assetID, args...);
	Debug::assetDebugWindow->addAsset(asset);
	m_instance->m_assets[assetID] = asset;
	return asset;
}

template<typename T>
inline T* AssetManager::loadAsset(std::string assetID, std::string filepath)
{
	auto it = m_instance->m_assets.find(assetID);
	if (it != m_instance->m_assets.end())
	{
		Debug::warning("Did not load asset with ID " + assetID + " because an asset with that ID already exists.");
		return nullptr;
	}

	T* asset = new T(m_instance->m_device, m_instance->m_context, assetID, filepath);
	if (!asset->loadAsset())
	{
		Debug::warning("Failed to load in asset with ID " + assetID + " from " + filepath);
		delete asset;
		return nullptr;
	}

	Debug::assetDebugWindow->addAsset(asset);
	m_instance->m_assets[assetID] = asset;
	return asset;
}

template<typename T>
inline T* AssetManager::getAsset(std::string assetID)
{
	auto it = m_instance->m_assets.find(assetID);
	if (it == m_instance->m_assets.end())
	{
		Debug::warning("Could not find asset with ID " + assetID);
		return nullptr;
	}

	return dynamic_cast<T*>(it->second);
}

template<typename T>
inline void AssetManager::unloadAsset(std::string assetID)
{
	auto it = m_instance->m_assets.find(assetID);
	if (it == m_instance->m_assets.end())
	{
		Debug::warning("Did not unload asset with ID " + assetID + " because an asset with that ID could not be found.");
		return;
	}

	Debug::assetDebugWindow->removeAsset(dynamic_cast<T*>(it->second));

	delete it->second;
	m_instance->m_assets.erase(assetID);	
}
