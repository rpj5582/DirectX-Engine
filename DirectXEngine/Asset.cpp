#include "Asset.h"

#include "Debug.h"

Asset::Asset(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath)
{
	m_device = device;
	m_context = context;
	m_assetID = assetID;
	m_filepath = filepath;
}

Asset::~Asset()
{
}

void Asset::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	writer.Key("id");
	writer.String(m_assetID.c_str());

	writer.Key("path");
	writer.String(m_filepath.c_str());
}

std::string Asset::getAssetID() const
{
	return m_assetID;
}

std::string Asset::getFilepath() const
{
	return m_filepath;
}

bool Asset::wasLoadedFromFile() const
{
	if (m_filepath.empty()) return false;

	return true;
}
