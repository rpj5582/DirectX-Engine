#pragma once
#include "SimpleShader.h"

class VertexShader : public SimpleVertexShader
{
public:
	VertexShader(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	~VertexShader();

	bool loadFromFile() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;
};

