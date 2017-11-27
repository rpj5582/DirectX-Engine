#pragma once
#include "Asset.h"

#include "SimpleShader.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "Texture.h"
#include "Sampler.h"

struct MaterialSettings
{
	std::string diffuseTextureID;
	std::string specularTextureID;
	std::string normalTextureID;
	std::string samplerID;
};

class Material : public Asset
{
public:
	Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID);
	~Material();

	bool create(VertexShader* vertexShader, PixelShader* pixelShader, const MaterialSettings& materialSettings);
	bool loadFromFile() override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	VertexShader* getVertexShader() const;
	PixelShader* getPixelShader() const;

	MaterialSettings getMaterialSettings() const;
	void setMaterialSettings(const MaterialSettings& settings);

	void useMaterial();

private:
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;
	Texture* m_diffuseTexture;
	Texture* m_specularTexture;
	Texture* m_normalTexture;
	Sampler* m_sampler;
};