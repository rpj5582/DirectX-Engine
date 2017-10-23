#include "Texture.h"

#include "../Debug/Debug.h"
#include "WICTextureLoader.h"

using namespace DirectX;

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, unsigned int hexColor) : Asset(device, context, assetID, "")
{
	createSolidColor(hexColor);
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
	m_textureSRV = nullptr;
}

Texture::~Texture()
{
	if(m_textureSRV)
		m_textureSRV->Release();
}

void Texture::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("texture");
}

bool Texture::loadAsset()
{
	HRESULT hr = S_OK;
	std::wstring wideFilePath = std::wstring(m_filepath.begin(), m_filepath.end());
	hr = CreateWICTextureFromFile(m_device, m_context, wideFilePath.c_str(), nullptr, &m_textureSRV);
	if (FAILED(hr) || !m_textureSRV)
	{
		char* errorMsg;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMsg, 0, NULL) != 0)
		{
			std::string errorString = std::string(errorMsg);
			errorString.pop_back();
			errorString.pop_back();
			Debug::warning("Failed to load texture with ID " + m_assetID + " at Assets/Textures/" + m_filepath + ": " + errorString);
		}
		else
			Debug::warning("Failed to load texture with ID " + m_assetID + " at Assets/Textures/" + m_filepath + ": Unable to find error description.");

		return false;
	}

	return true;
}

void Texture::createSolidColor(unsigned int hexColor)
{
	unsigned int color = hexColor;

	// Creates the texture resource
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA pixelData = {};
	pixelData.pSysMem = &color;
	pixelData.SysMemPitch = sizeof(unsigned int);
	pixelData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = S_OK;
	m_device->CreateTexture2D(&desc, &pixelData, &texture);
	if (FAILED(hr))
	{
		Debug::error("Failed to create texture resource for ID " + m_assetID + ".");
		return;
	}

	// Creates the texture shader resource view
	D3D11_TEX2D_SRV srvDescTexture2D;
	srvDescTexture2D.MipLevels = -1;
	srvDescTexture2D.MostDetailedMip = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D = srvDescTexture2D;

	hr = m_device->CreateShaderResourceView(texture, &srvDesc, &m_textureSRV);
	if (FAILED(hr))
	{
		Debug::error("Failed to create texture shader resource view for ID " + m_assetID + ".");
		texture->Release();
		return;
	}

	texture->Release();
}

ID3D11ShaderResourceView* Texture::getSRV() const
{
	return m_textureSRV;
}
