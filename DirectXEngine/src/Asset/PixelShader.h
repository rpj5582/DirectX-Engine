#pragma once
#include "SimpleShader.h"

class PixelShader : public SimplePixelShader
{
public:
	PixelShader(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	~PixelShader();

	bool loadAsset() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;
};

