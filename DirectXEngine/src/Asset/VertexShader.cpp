#include "VertexShader.h"

#include "../Debug/Debug.h"

VertexShader::VertexShader(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : SimpleVertexShader(assetID, filepath, device, context)
{
}

VertexShader::~VertexShader()
{
}

void VertexShader::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("vertexShader");
}

bool VertexShader::loadAsset()
{
	std::wstring filePathStr = std::wstring(m_filepath.begin(), m_filepath.end());
	if (!LoadShaderFile(filePathStr.c_str()))
	{
		Debug::warning("Failed to load vertex shader with ID " + m_assetID + " at file location " + m_filepath);
		return false;
	}

	return true;
}
