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
	deleteAssets();
	m_instance = nullptr;
}

bool AssetManager::init()
{
	return loadDefaultAssets();
}

void AssetManager::loadFromJSON(rapidjson::Value& assetsArray)
{
	unloadAllAssets();

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
			std::string specularID = DEFAULT_TEXTURE_SPECULAR;
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
		if (it->second->getAssetID() == DEFAULT_SHADER_VERTEX || it->second->getAssetID() == DEFAULT_SHADER_PIXEL || it->second->getAssetID() == DEFAULT_TEXTURE_DIFFUSE
			|| it->second->getAssetID() == DEFAULT_TEXTURE_SPECULAR || it->second->getAssetID() == DEFAULT_TEXTURE_NORMAL || it->second->getAssetID() == DEFAULT_TEXTURE_GUI
			|| it->second->getAssetID() == DEFAULT_MATERIAL || it->second->getAssetID() == DEFAULT_SAMPLER || it->second->getAssetID() == DEFAULT_FONT) continue;

		writer.StartObject();
		it->second->saveToJSON(writer);
		writer.EndObject();
	}
}

void AssetManager::unloadAllAssets()
{
	m_instance->deleteAssets();
	m_instance->loadDefaultAssets();
}

bool AssetManager::loadDefaultAssets()
{
	// Loads the default shaders
	VertexShader* vertexShader = loadAsset<VertexShader>(DEFAULT_SHADER_VERTEX, "Assets/Shaders/VertexShader.cso");
	if (!vertexShader) return false;

	PixelShader* pixelShader = loadAsset<PixelShader>(DEFAULT_SHADER_PIXEL, "Assets/Shaders/PixelShader.cso");
	if (!pixelShader) return false;

	// Creates the default textures
	if (!createAsset<Texture>(DEFAULT_TEXTURE_DIFFUSE, 0xff808080)) return false;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_SPECULAR, 0xffffffff)) return false;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_NORMAL, 0xffff8080)) return false;
	if (!createAsset<Texture>(DEFAULT_TEXTURE_GUI, 0xffffffff)) return false;

	// Creates the default sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (!createAsset<Sampler>(DEFAULT_SAMPLER, samplerDesc)) return false;

	// Creates the default material
	MaterialSettings defaultMaterialSettings;
	defaultMaterialSettings.diffuseTextureID = DEFAULT_TEXTURE_DIFFUSE;
	defaultMaterialSettings.specularTextureID = DEFAULT_TEXTURE_SPECULAR;
	defaultMaterialSettings.normalTextureID = DEFAULT_TEXTURE_NORMAL;
	defaultMaterialSettings.samplerID = DEFAULT_SAMPLER;
	if (!createAsset<Material>(DEFAULT_MATERIAL, vertexShader, pixelShader, defaultMaterialSettings)) return false;

	// Load Arial font
	if (!loadAsset<Font>(DEFAULT_FONT, "Assets/Fonts/Arial_16pt.spritefont")) return false;

	return true;
}

void AssetManager::deleteAssets()
{
	for (auto it = m_assets.begin(); it != m_assets.end(); it++)
	{
		delete it->second;
	}
	m_assets.clear();
}
