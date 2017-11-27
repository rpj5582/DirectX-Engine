#include "Material.h"

#include "AssetManager.h"

#include "rapidjson/error/en.h"
#include <fstream>

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_diffuseTexture = nullptr;
	m_specularTexture = nullptr;
	m_normalTexture = nullptr;
	m_sampler = nullptr;
}

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID) : Asset(device, context, assetID, "")
{
	m_diffuseTexture = nullptr;
	m_specularTexture = nullptr;
	m_normalTexture = nullptr;
	m_sampler = nullptr;
}

Material::~Material()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_diffuseTexture = nullptr;
	m_specularTexture = nullptr;
	m_normalTexture = nullptr;
	m_sampler = nullptr;
}

bool Material::create(VertexShader* vertexShader, PixelShader* pixelShader, const MaterialSettings& materialSettings)
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;
	setMaterialSettings(materialSettings);

	return true;
}

void Material::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("material");
}

VertexShader* Material::getVertexShader() const
{
	return m_vertexShader;
}

PixelShader* Material::getPixelShader() const
{
	return m_pixelShader;
}

bool Material::loadFromFile()
{
	// Load the json file
	std::ifstream ifs(m_filepath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::warning("Failed to load file from " + m_filepath + " when creating material with ID " + m_assetID + ": " + errString);
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

		Debug::error("Failed to load material at " + m_filepath + " because there was a parse error at character " + std::to_string(result.Offset()) + ": " + std::string(errorMessage));
		delete[] jsonStringBuffer;
		return false;
	}

	delete[] jsonStringBuffer;

	ifs.close();

	rapidjson::Value::MemberIterator vertexShader = dom.FindMember("vertexShader");
	if (vertexShader != dom.MemberEnd())
	{
		m_vertexShader = AssetManager::getAsset<VertexShader>(vertexShader->value.GetString());
	}
	else
	{
		m_vertexShader = AssetManager::getAsset<VertexShader>(DEFAULT_SHADER_VERTEX);
	}

	rapidjson::Value::MemberIterator pixelShader = dom.FindMember("pixelShader");
	if (pixelShader != dom.MemberEnd())
	{
		m_pixelShader = AssetManager::getAsset<PixelShader>(pixelShader->value.GetString());
	}
	else
	{
		m_pixelShader = AssetManager::getAsset<PixelShader>(DEFAULT_SHADER_PIXEL);
	}

	MaterialSettings settings;

	rapidjson::Value::MemberIterator diffuse = dom.FindMember("diffuse");
	if (diffuse != dom.MemberEnd())
	{
		settings.diffuseTextureID = diffuse->value.GetString();
	}
	else
	{
		settings.diffuseTextureID = DEFAULT_TEXTURE_DIFFUSE;
	}

	rapidjson::Value::MemberIterator specular = dom.FindMember("specular");
	if (specular != dom.MemberEnd())
	{
		settings.specularTextureID = specular->value.GetString();
	}
	else
	{
		settings.specularTextureID = DEFAULT_TEXTURE_WHITE;
	}

	rapidjson::Value::MemberIterator normal = dom.FindMember("normal");
	if (normal != dom.MemberEnd())
	{
		settings.normalTextureID = normal->value.GetString();
	}
	else
	{
		settings.normalTextureID = DEFAULT_TEXTURE_NORMAL;
	}

	rapidjson::Value::MemberIterator sampler = dom.FindMember("sampler");
	if (sampler != dom.MemberEnd())
	{
		settings.samplerID = sampler->value.GetString();
	}
	else
	{
		settings.samplerID = DEFAULT_SAMPLER;
	}

	setMaterialSettings(settings);

	return true;
}

MaterialSettings Material::getMaterialSettings() const
{
	MaterialSettings settings;
	settings.diffuseTextureID = m_diffuseTexture->getAssetID();
	settings.specularTextureID = m_specularTexture->getAssetID();
	settings.normalTextureID = m_normalTexture->getAssetID();
	settings.samplerID = m_sampler->getAssetID();

	return settings;
}

void Material::setMaterialSettings(const MaterialSettings& settings)
{
	Texture* diffuse = AssetManager::getAsset<Texture>(settings.diffuseTextureID);
	if (!diffuse)
	{
		diffuse = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_DIFFUSE);
	}

	Texture* specular = AssetManager::getAsset<Texture>(settings.specularTextureID);
	if (!specular)
	{
		specular = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_WHITE);
	}

	Texture* normal = AssetManager::getAsset<Texture>(settings.normalTextureID);
	if (!normal)
	{
		normal = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_NORMAL);
	}

	Sampler* sampler = AssetManager::getAsset<Sampler>(settings.samplerID);
	if (!sampler)
	{
		sampler = AssetManager::getAsset<Sampler>(DEFAULT_SAMPLER);
	}

	m_diffuseTexture = diffuse;
	m_specularTexture = specular;
	m_normalTexture = normal;
	m_sampler = sampler;
}

void Material::useMaterial()
{
	if(m_vertexShader)
		m_vertexShader->SetShader();

	if (m_pixelShader)
	{
		m_pixelShader->SetShader();

		ID3D11ShaderResourceView* diffuseSRV = m_diffuseTexture->getSRV();
		if (!diffuseSRV)
		{
			Debug::error("A texture used by a material must have a shader resource view.");
			return;
		}

		ID3D11ShaderResourceView* specularSRV = m_specularTexture->getSRV();
		if (!specularSRV)
		{
			Debug::error("A texture used by a material must have a shader resource view.");
			return;
		}

		ID3D11ShaderResourceView* normalSRV = m_normalTexture->getSRV();
		if (!normalSRV)
		{
			Debug::error("A texture used by a material must have a shader resource view.");
			return;
		}

		m_pixelShader->SetShaderResourceView("diffuseTexture", diffuseSRV);
		m_pixelShader->SetShaderResourceView("specularTexture", specularSRV);
		m_pixelShader->SetShaderResourceView("normalTexture", normalSRV);
		m_pixelShader->SetSamplerState("materialSampler", m_sampler->getSamplerState());
	}
}
