#pragma once
#include "Asset.h"

class Texture : public Asset
{
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, unsigned int hexColor);
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	~Texture();

	bool loadAsset() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	ID3D11ShaderResourceView* getSRV() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	void createSolidColor(unsigned int hexColor);

	ID3D11ShaderResourceView* m_textureSRV;
	unsigned int m_width;
	unsigned int m_height;
};

