#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader,
	ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV, ID3D11ShaderResourceView* normalTextureSRV, ID3D11SamplerState* sampler)
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;

	m_diffuseTextureSRV = diffuseTextureSRV;
	m_specularTextureSRV = specularTextureSRV;
	m_normalTextureSRV = normalTextureSRV;
	m_sampler = sampler;
}

Material::~Material()
{
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
