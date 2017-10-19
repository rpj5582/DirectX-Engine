#pragma once
#include "Asset.h"

#include "SimpleShader.h"

#include "VertexShader.h"
#include "PixelShader.h"

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
	Material(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, VertexShader* vertexShader, PixelShader* pixelShader, const MaterialSettings& materialSettings);
	~Material();

	bool loadAsset() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	VertexShader* getVertexShader() const;
	PixelShader* getPixelShader() const;

	MaterialSettings getMaterialSettings() const;
	void setMaterialSettings(const MaterialSettings& settings);

	void useMaterial();

private:
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	MaterialSettings m_materialSettings;
};