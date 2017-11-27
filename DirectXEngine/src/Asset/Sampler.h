#pragma once
#include "Asset.h"

class Sampler : public Asset
{
public:
	Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID);
	Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	~Sampler();

	bool create(const D3D11_SAMPLER_DESC& samplerDesc);
	bool loadFromFile() override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	ID3D11SamplerState* getSamplerState() const;

private:
	ID3D11SamplerState* m_sampler;
	D3D11_SAMPLER_DESC m_samplerDesc;
};

