#pragma once
#include "Asset.h"

class Sampler : public Asset
{
public:
	Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, const D3D11_SAMPLER_DESC& samplerDesc);
	Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	~Sampler();

	bool loadAsset() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	ID3D11SamplerState* getSampler() const;

private:
	void create(const D3D11_SAMPLER_DESC& samplerDesc);

	ID3D11SamplerState* m_sampler;
};

