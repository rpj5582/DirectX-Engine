#include "Material.h"

#include "AssetManager.h"

Material::Material(std::string vertexShaderID, std::string pixelShaderID, std::string diffuseTextureID,
	std::string specularTextureID, std::string normalTextureID, std::string samplerID)
{
	m_vertexShaderID = vertexShaderID;
	m_pixelShaderID = pixelShaderID;
	m_diffuseTextureID = diffuseTextureID;
	m_specularTextureID = specularTextureID;
	m_normalTextureID = normalTextureID;
	m_samplerID = samplerID;

	m_vertexShader = AssetManager::getVertexShader(vertexShaderID);
	m_pixelShader = AssetManager::getPixelShader(pixelShaderID);

	m_diffuseTextureSRV = AssetManager::getTexture(diffuseTextureID);
	m_specularTextureSRV = AssetManager::getTexture(specularTextureID);
	m_normalTextureSRV = AssetManager::getTexture(normalTextureID);
	m_sampler = AssetManager::getSampler(samplerID);
}

Material::~Material()
{
}

std::string Material::getVertexShaderID() const
{
	return m_vertexShaderID;
}

std::string Material::getPixelShaderID() const
{
	return m_pixelShaderID;
}

std::string Material::getDiffuseTextureID() const
{
	return m_diffuseTextureID;
}

std::string Material::getSpecularTextureID() const
{
	return m_specularTextureID;
}

std::string Material::getNormalTextureID() const
{
	return m_normalTextureID;
}

std::string Material::getSamplerID() const
{
	return m_samplerID;
}

SimpleVertexShader* Material::getVertexShader() const
{
	return m_vertexShader;
}

SimplePixelShader* Material::getPixelShader() const
{
	return m_pixelShader;
}

void Material::useMaterial()
{
	m_vertexShader->SetShader();
	m_pixelShader->SetShader();

	m_pixelShader->SetShaderResourceView("diffuseTexture", m_diffuseTextureSRV);
	m_pixelShader->SetShaderResourceView("normalTexture", m_normalTextureSRV);
	m_pixelShader->SetShaderResourceView("specularTexture", m_specularTextureSRV);
	m_pixelShader->SetSamplerState("materialSampler", m_sampler);
}

ID3D11ShaderResourceView* Material::getDiffuseTextureSRV() const
{
	return m_diffuseTextureSRV;
}
