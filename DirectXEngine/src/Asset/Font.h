#pragma once
#include "Asset.h"

#include <SpriteFont.h>

class Font : public Asset
{
public:
	Font(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	Font(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, DirectX::SpriteFont* spriteFont);
	~Font();

	bool loadFromFile() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	DirectX::SpriteFont* getSpriteFont() const;

private:
	DirectX::SpriteFont* m_spriteFont;
};

