#pragma once

#include "VertexShader.h"
#include "PixelShader.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Font.h"
#include "Sampler.h"

#include <unordered_map>

#define DEFAULT_SHADER_VERTEX "defaultVertex"
#define BASIC_SHADER_VERTEX "basicVertex"
#define DEFAULT_SHADER_PIXEL "defaultPixel"

#define DEFAULT_TEXTURE_DIFFUSE "defaultDiffuse"
#define DEFAULT_TEXTURE_WHITE "defaultWhite"
#define DEFAULT_TEXTURE_RED "defaultRed"
#define DEFAULT_TEXTURE_NORMAL "defaultNormal"
#define DEFAULT_TEXTURE_SHADOWMAP "defaultShadowMap"

#define DEFAULT_MATERIAL "defaultMaterial"
#define DEFAULT_RED_MATERIAL "defaultRedMaterial"

#define DEFAULT_MODEL_CUBE "defaultCube"
#define DEFAULT_MODEL_SPHERE "defaultSphere"
#define DEFAULT_MODEL_CAPSULE "defaultCapsule"

#define DEFAULT_SAMPLER "defaultSampler"
#define SHADOWMAP_SAMPLER "shadowMapSampler"

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
	static void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	template<typename T, typename... Args>
	static T* createAsset(std::string assetID, Args... args);

	template<typename T, typename... Args>
	static T* loadAsset(std::string assetID, std::string filepath, Args... args);

	template<typename T>
	static T* getAsset(std::string assetID);

	static std::vector<Asset*> getAllAssets();

	template<typename T>
	static std::vector<T*> getAllAssets();

	template<typename T>
	static void unloadAsset(Asset* asset);

	static void unloadAllAssets();

	static bool isDefaultAsset(std::string assetName);

private:
	bool loadDefaultAssets();
	void deleteAssets(bool defaults);

	static AssetManager* m_instance;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::unordered_map<std::string, Asset*> m_assets;
};

template<typename T, typename... Args>
inline T* AssetManager::createAsset(std::string assetID, Args... args)
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	auto it = m_instance->m_assets.find(assetID);
	if (it != m_instance->m_assets.end())
	{
		Debug::warning("Did not create asset with ID " + assetID + " because an asset with that ID already exists.");
		return nullptr;
	}

	T* asset = new T(m_instance->m_device, m_instance->m_context, assetID);
	if (!asset->create(args...))
	{
		Debug::warning("Failed to create asset with ID " + assetID + ".");
		delete asset;
		return nullptr;
	}

	m_instance->m_assets[assetID] = asset;
	return asset;
}

template<typename T, typename... Args>
inline T* AssetManager::loadAsset(std::string assetID, std::string filepath, Args... args)
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	auto it = m_instance->m_assets.find(assetID);
	if (it != m_instance->m_assets.end())
	{
		Debug::warning("Did not load asset with ID " + assetID + " because an asset with that ID already exists.");
		return nullptr;
	}

	T* asset = new T(m_instance->m_device, m_instance->m_context, assetID, filepath, args...);
	if (!asset->loadFromFile())
	{
		Debug::warning("Failed to load in asset with ID " + assetID + " from " + filepath);
		delete asset;
		return nullptr;
	}

	m_instance->m_assets[assetID] = asset;
	return asset;
}

template<typename T>
inline T* AssetManager::getAsset(std::string assetID)
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	auto it = m_instance->m_assets.find(assetID);
	if (it == m_instance->m_assets.end())
	{
		Debug::warning("Could not find asset with ID " + assetID);
		return nullptr;
	}

	return dynamic_cast<T*>(it->second);
}

template<typename T>
std::vector<T*> AssetManager::getAllAssets()
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	std::vector<T*> assetVector;

	for (auto it = m_instance->m_assets.begin(); it != m_instance->m_assets.end(); it++)
	{
		T* asset = getAsset<T>(it->first);
		if (asset) assetVector.push_back(asset);
	}

	return assetVector;
}

template<typename T>
inline void AssetManager::unloadAsset(Asset* asset)
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	if (!asset)
	{
		Debug::warning("Could not unload asset because the asset was nullptr.");
		return;
	}

	std::string assetID = asset->getAssetID();
	delete asset;
	m_instance->m_assets.erase(assetID);
}
