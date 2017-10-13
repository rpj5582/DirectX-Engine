#pragma once

#include "SimpleShader.h"

class Material
{
public:
	Material(std::string vertexShaderID, std::string pixelShaderID, std::string diffuseTextureID,
		std::string specularTextureID, std::string normalTextureID, std::string samplerID);
	~Material();

	std::string getVertexShaderID() const;
	std::string getPixelShaderID() const;
	std::string getDiffuseTextureID() const;
	std::string getSpecularTextureID() const;
	std::string getNormalTextureID() const;
	std::string getSamplerID() const;

	SimpleVertexShader* getVertexShader() const;
	SimplePixelShader* getPixelShader() const;
	ID3D11ShaderResourceView* getDiffuseTextureSRV() const;

	void useMaterial();

private:
	std::string m_vertexShaderID;
	std::string m_pixelShaderID;
	std::string m_diffuseTextureID;
	std::string m_specularTextureID;
	std::string m_normalTextureID;
	std::string m_samplerID;

	SimpleVertexShader* m_vertexShader;
	SimplePixelShader* m_pixelShader;

	ID3D11ShaderResourceView* m_diffuseTextureSRV;
	ID3D11ShaderResourceView* m_specularTextureSRV;
	ID3D11ShaderResourceView* m_normalTextureSRV;
	ID3D11SamplerState* m_sampler;
};