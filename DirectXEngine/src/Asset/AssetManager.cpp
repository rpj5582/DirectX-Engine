#include "AssetManager.h"

#include "../Util.h"

AssetManager* AssetManager::m_instance = nullptr;

AssetManager::AssetManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	if (!m_instance) m_instance = this;
	else return;

	m_device = device;
	m_context = context;

	m_assets = std::unordered_map<std::string, Asset*>();
}

AssetManager::~AssetManager()
{
	deleteAssets(true);
	m_instance = nullptr;
}

bool AssetManager::init()
{
	return loadDefaultAssets();
}

void AssetManager::loadFromJSON(rapidjson::Value& assetsArray)
{
	// Queue up all materials since they can only be loaded once all other assets have been loaded.
	std::vector<std::pair<std::string, std::string>> materials;

	for (rapidjson::SizeType i = 0; i < assetsArray.Size(); i++)
	{
		rapidjson::Value& asset = assetsArray[i];
		rapidjson::Value& assetType = asset["type"];

		rapidjson::Value& idRef = asset["id"];
		std::string id = idRef.GetString();

		rapidjson::Value::MemberIterator pathIter = asset.FindMember("path");
		std::string path = "";
		if (pathIter != asset.MemberEnd())
		{
			path = pathIter->value.GetString();
		}

		switch (stringHash(assetType.GetString()))
		{
		case stringHash("texture"):
		{
			loadAsset<Texture>(id, path);
			break;
		}

		case stringHash("model"):
		{
			loadAsset<Mesh>(id, path);
			break;
		}

		case stringHash("font"):
		{
			loadAsset<Font>(id, path);
			break;
		}

		case stringHash("vertexShader"):
		{
			loadAsset<VertexShader>(id, path);
			break;
		}

		case stringHash("pixelShader"):
		{
			loadAsset<PixelShader>(id, path);
			break;
		}

		case stringHash("material"):
		{
			materials.push_back(std::pair<std::string, std::string>(id, path));
			break;
		}

		case stringHash("sampler"):
		{
			loadAsset<Sampler>(id, path);
			break;
		}

		default:
			Debug::warning("Invalid asset type " + std::string(assetType.GetString()) + ", skipping.");
			break;
		}
	}

	// Load all queued materials.
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		loadAsset<Material>(materials[i].first, materials[i].second);
	}
}

void AssetManager::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	for (auto it = m_instance->m_assets.begin(); it != m_instance->m_assets.end(); it++)
	{
		// Don't save procedurally generated assets.
		if (it->second->getFilepath() == "") continue;

		// Don't save default objects since they will be automatically loaded at the start of the program.
		if (m_instance->isDefaultAsset(it->second->getAssetID())) continue;

		writer.StartObject();
		it->second->saveToJSON(writer);
		writer.EndObject();
	}
}

void AssetManager::unloadAllAssets()
{
	m_instance->deleteAssets(false);
}

bool AssetManager::loadDefaultAssets()
{
	// Loads the default shaders
	VertexShader* defaultVertexShader = loadAsset<VertexShader>(DEFAULT_SHADER_VERTEX, "Assets/Shaders/VertexShader.cso");
	if (!defaultVertexShader) return false;

	PixelShader* defaultPixelShader = loadAsset<PixelShader>(DEFAULT_SHADER_PIXEL, "Assets/Shaders/PixelShader.cso");
	if (!defaultPixelShader) return false;

	VertexShader* basicVertexShader = loadAsset<VertexShader>(BASIC_SHADER_VERTEX, "Assets/Shaders/BasicVertexShader.cso");
	if (!basicVertexShader) return false;

	// Creates the default textures
	if (!createAsset<Texture>(DEFAULT_TEXTURE_DIFFUSE, 0xff808080)) return false;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_WHITE, 0xffffffff)) return false;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_NORMAL, 0xffff8080)) return false;
	
	TextureParameters shadowMapParameters = {};
	shadowMapParameters.textureFormat = DXGI_FORMAT_R32_TYPELESS;
	shadowMapParameters.shaderResourceViewFormat = DXGI_FORMAT_R32_FLOAT;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_SHADOWMAP, 0xffffffff, shadowMapParameters)) return false;

	// Creates the default sampler
	D3D11_SAMPLER_DESC defaultSamplerDesc = {};
	defaultSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (!createAsset<Sampler>(DEFAULT_SAMPLER, defaultSamplerDesc)) return false;

	// Creates the shadow map sampler
	D3D11_SAMPLER_DESC shadowMapSamplerDesc = {};
	shadowMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowMapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowMapSamplerDesc.BorderColor[0] = 1.0f;
	shadowMapSamplerDesc.BorderColor[1] = 1.0f;
	shadowMapSamplerDesc.BorderColor[2] = 1.0f;
	shadowMapSamplerDesc.BorderColor[3] = 1.0f;
	shadowMapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (!createAsset<Sampler>(SHADOWMAP_SAMPLER, shadowMapSamplerDesc)) return false;

	// Creates the default material
	MaterialSettings defaultMaterialSettings;
	defaultMaterialSettings.diffuseTextureID = DEFAULT_TEXTURE_DIFFUSE;
	defaultMaterialSettings.specularTextureID = DEFAULT_TEXTURE_WHITE;
	defaultMaterialSettings.normalTextureID = DEFAULT_TEXTURE_NORMAL;
	defaultMaterialSettings.samplerID = DEFAULT_SAMPLER;
	if (!createAsset<Material>(DEFAULT_MATERIAL, defaultVertexShader, defaultPixelShader, defaultMaterialSettings)) return false;

	// Load Arial font
	if (!loadAsset<Font>(DEFAULT_FONT, "Assets/Fonts/Arial_16pt.spritefont")) return false;

#if defined(DEBUG) || defined(_DEBUG)
	// Textures
	if (!loadAsset<Texture>(DEBUG_TEXTURE_DEFAULTICON, "Assets/Debug/defaulticon.png")) return false;
	if (!loadAsset<Texture>(DEBUG_TEXTURE_LIGHTICON, "Assets/Debug/lighticon.png")) return false;
	if (!loadAsset<Texture>(DEBUG_TEXTURE_CAMERAICON, "Assets/Debug/cameraicon.png")) return false;
#endif
	return true;
}

bool AssetManager::isDefaultAsset(std::string assetName)
{
	if (assetName == DEFAULT_SHADER_VERTEX || assetName == DEFAULT_SHADER_PIXEL || assetName == BASIC_SHADER_VERTEX
		|| assetName == DEFAULT_TEXTURE_DIFFUSE || assetName == DEFAULT_TEXTURE_WHITE || assetName == DEFAULT_TEXTURE_NORMAL || assetName == DEFAULT_TEXTURE_SHADOWMAP
		|| assetName == DEFAULT_MATERIAL
		|| assetName == DEFAULT_SAMPLER || assetName == SHADOWMAP_SAMPLER
		|| assetName == DEFAULT_FONT
		|| assetName == DEBUG_TEXTURE_DEFAULTICON || assetName == DEBUG_TEXTURE_CAMERAICON || assetName == DEBUG_TEXTURE_LIGHTICON) return true;

	return false;
}

void AssetManager::deleteAssets(bool defaults)
{
	std::unordered_map<std::string, Asset*> defaultAssets = {};

	for (auto it = m_assets.begin(); it != m_assets.end(); it++)
	{
		if (!defaults && isDefaultAsset(it->first))
			defaultAssets[it->first] = it->second;
		else
			delete it->second;
	}

	m_assets = defaultAssets;
}
