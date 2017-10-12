#pragma once

#include "SimpleShader.h"

class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader,
		ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV, ID3D11ShaderResourceView* normalTextureSRV, ID3D11SamplerState* sampler);
	~Material();

	SimpleVertexShader* getVertexShader() const;
	SimplePixelShader* getPixelShader() const;

	void useMaterial();

	ID3D11ShaderResourceView* getDiffuseTextureSRV() const;

private:
	SimpleVertexShader* m_vertexShader;
	SimplePixelShader* m_pixelShader;

	ID3D11ShaderResourceView* m_diffuseTextureSRV;
	ID3D11ShaderResourceView* m_specularTextureSRV;
	ID3D11ShaderResourceView* m_normalTextureSRV;
	ID3D11SamplerState* m_sampler;
};