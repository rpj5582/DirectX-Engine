#include "PixelShader.h"

#include "../Debug/Debug.h"

PixelShader::PixelShader(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : SimplePixelShader(assetID, filepath, device, context)
{
}

PixelShader::~PixelShader()
{
}

void PixelShader::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("pixelShader");
}

bool PixelShader::create()
{
	Debug::error("Pixel Shaders cannot be created procedurally.");
	return false;
}

bool PixelShader::loadFromFile()
{
	std::wstring filePathStr = std::wstring(m_filepath.begin(), m_filepath.end());
	if (!LoadShaderFile(filePathStr.c_str()))
	{
		Debug::warning("Failed to load pixel shader with ID " + m_assetID + " at file location " + m_filepath);
		return false;
	}

	return true;
}
