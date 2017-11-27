#pragma once
#include "Asset.h"

struct TextureParameters
{
	unsigned int mipLevels = 0;
	D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE;
	long bindFlags = D3D11_BIND_SHADER_RESOURCE;
	DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT shaderResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depthStencilViewFormat = DXGI_FORMAT_UNKNOWN;
};

class Texture : public Asset
{
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID);
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath, TextureParameters parameters = {});
	~Texture();

	bool create(unsigned int width, unsigned int height, TextureParameters parameters = {});
	bool create(unsigned int hexColor, TextureParameters parameters = {});
	bool loadFromFile() override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	ID3D11ShaderResourceView* getSRV() const;
	ID3D11DepthStencilView* getDSV() const;

	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:
	bool createEmpty();
	bool createSolidColor(unsigned int hexColor);

	ID3D11ShaderResourceView* m_textureSRV;
	ID3D11DepthStencilView* m_textureDSV;

	unsigned int m_width;
	unsigned int m_height;
	TextureParameters m_parameters;
};

