#pragma once

#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"

#include <d3d11.h>
#include <string>

class Asset
{
public:
	friend class AssetManager;

	virtual bool loadFromFile() = 0;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	std::string getAssetID() const;
	std::string getFilepath() const;

	bool wasLoadedFromFile() const;

protected:
	Asset(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	virtual ~Asset();

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	std::string m_assetID;
	std::string m_filepath;
};

