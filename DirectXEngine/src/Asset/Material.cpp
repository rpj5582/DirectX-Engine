#include "Material.h"

#include "AssetManager.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
}

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, VertexShader* vertexShader, PixelShader* pixelShader, const MaterialSettings& materialSettings) : Asset(device, context, assetID, "")
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;

	setMaterialSettings(materialSettings);
}

Material::~Material()
{
}

void Material::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("material");

	writer.Key("vertexShader");
	writer.String(m_vertexShader->getAssetID().c_str());

	writer.Key("pixelShader");
	writer.String(m_pixelShader->getAssetID().c_str());

	writer.Key("diffuse");
	writer.String(m_materialSettings.diffuseTextureID.c_str());

	writer.Key("specular");
	writer.String(m_materialSettings.specularTextureID.c_str());

	writer.Key("normal");
	writer.String(m_materialSettings.normalTextureID.c_str());

	writer.Key("sampler");
	writer.String(m_materialSettings.samplerID.c_str());
}

VertexShader* Material::getVertexShader() const
{
	return m_vertexShader;
}

PixelShader* Material::getPixelShader() const
{
	return m_pixelShader;
}

bool Material::loadAsset()
{
	Debug::error("Loading materials from file is not yet implemented.");
	return false;
}

MaterialSettings Material::getMaterialSettings() const
{
	return m_materialSettings;
}

void Material::setMaterialSettings(const MaterialSettings& settings)
{
	m_materialSettings = settings;
}

void Material::useMaterial()
{
	if(m_vertexShader)
		m_vertexShader->SetShader();

	if (m_pixelShader)
	{
		m_pixelShader->SetShader();

		Texture* diffuseTextureSRV = AssetManager::getAsset<Texture>(m_materialSettings.diffuseTextureID);
		Texture* specularTextureSRV = AssetManager::getAsset<Texture>(m_materialSettings.specularTextureID);
		Texture* normalTextureSRV = AssetManager::getAsset<Texture>(m_materialSettings.normalTextureID);
		Sampler* sampler = AssetManager::getAsset<Sampler>(m_materialSettings.samplerID);

		m_pixelShader->SetShaderResourceView("diffuseTexture", diffuseTextureSRV->getSRV());
		m_pixelShader->SetShaderResourceView("specularTexture", specularTextureSRV->getSRV());
		m_pixelShader->SetShaderResourceView("normalTexture", normalTextureSRV->getSRV());
		m_pixelShader->SetSamplerState("materialSampler", sampler->getSampler());
	}
}
