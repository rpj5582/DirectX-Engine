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
			rapidjson::Value::MemberIterator diffuseIter = asset.FindMember("diffuse");
			rapidjson::Value::MemberIterator specularIter = asset.FindMember("specular");
			rapidjson::Value::MemberIterator normalIter = asset.FindMember("normal");
			rapidjson::Value::MemberIterator samplerIter = asset.FindMember("sampler");
			rapidjson::Value::MemberIterator vertexShaderIter = asset.FindMember("vertexShader");
			rapidjson::Value::MemberIterator pixelShaderIter = asset.FindMember("pixelShader");

			std::string diffuseID = DEFAULT_TEXTURE_DIFFUSE;
			std::string specularID = DEFAULT_TEXTURE_WHITE;
			std::string normalID = DEFAULT_TEXTURE_NORMAL;
			std::string samplerID = DEFAULT_SAMPLER;
			std::string vertexShaderID = DEFAULT_SHADER_VERTEX;
			std::string pixelShaderID = DEFAULT_SHADER_PIXEL;

			if (diffuseIter != asset.MemberEnd())
			{
				diffuseID = diffuseIter->value.GetString();
			}

			if (specularIter != asset.MemberEnd())
			{
				specularID = specularIter->value.GetString();
			}

			if (normalIter != asset.MemberEnd())
			{
				normalID = normalIter->value.GetString();
			}

			if (samplerIter != asset.MemberEnd())
			{
				samplerID = samplerIter->value.GetString();
			}

			if (vertexShaderIter != asset.MemberEnd())
			{
				vertexShaderID = vertexShaderIter->value.GetString();
			}

			if (pixelShaderIter != asset.MemberEnd())
			{
				pixelShaderID = pixelShaderIter->value.GetString();
			}

			VertexShader* vertexShader = getAsset<VertexShader>(vertexShaderID);
			PixelShader* pixelShader = getAsset<PixelShader>(pixelShaderID);

			MaterialSettings materialSettings;
			materialSettings.diffuseTextureID = diffuseID;
			materialSettings.specularTextureID = specularID;
			materialSettings.normalTextureID = normalID;
			materialSettings.samplerID = samplerID;

			createAsset<Material>(id, vertexShader, pixelShader, materialSettings);
			break;
		}

		case stringHash("sampler"):
		{
			rapidjson::Value::MemberIterator addressUIter = asset.FindMember("addressU");
			rapidjson::Value::MemberIterator addressVIter = asset.FindMember("addressV");
			rapidjson::Value::MemberIterator addressWIter = asset.FindMember("addressW");
			rapidjson::Value::MemberIterator filterIter = asset.FindMember("filter");
			rapidjson::Value::MemberIterator maxLODIter = asset.FindMember("maxLOD");

			std::string addressUString = "wrap";
			std::string addressVString = "wrap";
			std::string addressWString = "wrap";
			std::string filterString = "min_mag_mip_linear";
			float maxLOD = D3D11_FLOAT32_MAX;

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
				if(maxLODIter->value.IsFloat())
					maxLOD = maxLODIter->value.GetFloat();
				else
				{
					maxLOD = D3D11_FLOAT32_MAX;
				}
			}

			D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP;
			D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			switch (stringHash(addressUString.c_str()))
			{
			case stringHash("wrap"):
				break;

			case stringHash("border"):
				addressU = D3D11_TEXTURE_ADDRESS_BORDER;
					break;

			case stringHash("clamp"):
				addressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				break;

			case stringHash("mirror"):
				addressU = D3D11_TEXTURE_ADDRESS_MIRROR;
				break;

			case stringHash("mirror_once"):
				addressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
				break;

			default:
				Debug::warning("Invalid sampler address mode " + addressUString + " for sampler " + id);
				break;
			}

			switch (stringHash(addressVString.c_str()))
			{
			case stringHash("wrap"):
				break;

			case stringHash("border"):
				addressV = D3D11_TEXTURE_ADDRESS_BORDER;
				break;

			case stringHash("clamp"):
				addressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				break;

			case stringHash("mirror"):
				addressV = D3D11_TEXTURE_ADDRESS_MIRROR;
				break;

			case stringHash("mirror_once"):
				addressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
				break;

			default:
				Debug::warning("Invalid sampler address mode " + addressVString + " for sampler " + id);
				break;
			}

			switch (stringHash(addressWString.c_str()))
			{
			case stringHash("wrap"):
				break;

			case stringHash("border"):
				addressW = D3D11_TEXTURE_ADDRESS_BORDER;
				break;

			case stringHash("clamp"):
				addressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				break;

			case stringHash("mirror"):
				addressW = D3D11_TEXTURE_ADDRESS_MIRROR;
				break;

			case stringHash("mirror_once"):
				addressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
				break;

			default:
				Debug::warning("Invalid sampler address mode " + addressWString + " for sampler " + id);
				break;
			}

			// Don't support different sampler filters yet because there are too many options


			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.AddressU = addressU;
			samplerDesc.AddressV = addressV;
			samplerDesc.AddressW = addressW;
			samplerDesc.Filter = filter;
			samplerDesc.MaxLOD = maxLOD;

			createAsset<Sampler>(id, samplerDesc);
			break;
		}

		default:
			Debug::warning("Invalid asset type " + std::string(assetType.GetString()) + ", skipping.");
			break;
		}
	}
}

void AssetManager::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	for (auto it = m_instance->m_assets.begin(); it != m_instance->m_assets.end(); it++)
	{
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
	if (!createAsset<Texture>(DEFAULT_TEXTURE_GUI, 0xffffffff)) return false;

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
		|| assetName == DEFAULT_TEXTURE_DIFFUSE || assetName == DEFAULT_TEXTURE_WHITE || assetName == DEFAULT_TEXTURE_NORMAL || assetName == DEFAULT_TEXTURE_GUI
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
