#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader)
{
	m_vertexShader = vertexShader;
	m_pixelShader = pixelShader;
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
}
