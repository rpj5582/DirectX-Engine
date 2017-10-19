#include "Font.h"

#include "Debug.h"

using namespace DirectX;

Font::Font(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
	m_spriteFont = nullptr;
}

Font::Font(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, DirectX::SpriteFont* spriteFont) : Asset(device, context, assetID, "")
{
	m_spriteFont = spriteFont;
}

Font::~Font()
{
	delete m_spriteFont;
}

void Font::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("font");
}

DirectX::SpriteFont* Font::getSpriteFont() const
{
	return m_spriteFont;
}

bool Font::loadAsset()
{
	std::wstring wideFilePath = std::wstring(m_filepath.begin(), m_filepath.end());

	try
	{
		m_spriteFont = new SpriteFont(m_device, wideFilePath.c_str());
	}
	catch (std::exception e)
	{
		Debug::warning("Failed to load font " + m_assetID + " at " + m_filepath);
		return false;
	}

	return true;
}
