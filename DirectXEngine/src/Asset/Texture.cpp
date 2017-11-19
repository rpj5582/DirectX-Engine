#include "Texture.h"

#include "../Debug/Debug.h"
#include "WICTextureLoader.h"

using namespace DirectX;

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, unsigned int width, unsigned int height, TextureParameters parameters)
	: Asset(device, context, assetID, "")
{
	m_textureSRV = nullptr;
	m_textureDSV = nullptr;

	m_hexColor = -1;
	m_width = width;
	m_height = height;
	m_parameters = parameters;
	createEmpty(m_parameters);
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, unsigned int hexColor, unsigned int width, unsigned int height, TextureParameters parameters)
	: Asset(device, context, assetID, "")
{
	m_textureSRV = nullptr;
	m_textureDSV = nullptr;

	m_hexColor = hexColor;
	m_width = width;
	m_height = height;
	m_parameters = parameters;

	createSolidColor(m_hexColor, m_parameters);
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath, TextureParameters parameters)
	: Asset(device, context, assetID, filepath)
{
	m_textureSRV = nullptr;
	m_textureDSV = nullptr;

	m_hexColor = -1;
	m_width = 0;
	m_height = 0;
	m_parameters = parameters;
}

Texture::~Texture()
{
	if (m_textureSRV)
	{
		m_textureSRV->Release();
		m_textureSRV = nullptr;
	}

	if (m_textureDSV)
	{
		m_textureDSV->Release();
		m_textureDSV = nullptr;
	}
		
}

//void Texture::loadFromJSON(rapidjson::Value& asset)
//{
//	rapidjson::Value& width = asset["width"];
//	rapidjson::Value& height = asset["height"];
//	rapidjson::Value& hexColor = asset["hexColor"];
//	rapidjson::Value& mipLevels = asset["mipLevels"];
//	rapidjson::Value& usage = asset["usage"];
//	rapidjson::Value& bindFlags = asset["bindFlags"];
//	rapidjson::Value& textureFormat = asset["textureFormat"];
//	rapidjson::Value& srvFormat = asset["srvFormat"];
//	rapidjson::Value& dsvFormat = asset["dsvFormat"];
//
//	m_width = width.GetUint();
//	m_height = height.GetUint();
//	m_hexColor = hexColor.GetInt();
//	m_parameters.mipLevels = mipLevels.GetUint();
//	m_parameters.usage = (D3D11_USAGE)usage.GetInt();
//	m_parameters.bindFlags = (D3D11_BIND_FLAG)bindFlags.GetInt();
//	m_parameters.textureFormat = (DXGI_FORMAT)textureFormat.GetInt();
//	m_parameters.shaderResourceViewFormat = (DXGI_FORMAT)srvFormat.GetInt();
//	m_parameters.depthStencilViewFormat = (DXGI_FORMAT)dsvFormat.GetInt();
//
//	if (m_hexColor >= 0)
//	{
//		createSolidColor(m_hexColor, m_parameters);
//	}
//	else
//	{
//		createEmpty(m_parameters);
//	}
//}

void Texture::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("texture");

	writer.Key("width");
	writer.Uint(m_width);
	
	writer.Key("height");
	writer.Uint(m_height);

	writer.Key("hexColor");
	writer.Int(m_hexColor);

	writer.Key("mipLevels");
	writer.Uint(m_parameters.mipLevels);

	writer.Key("usage");
	writer.Int(m_parameters.usage);

	writer.Key("bindFlags");
	writer.Int(m_parameters.bindFlags);

	writer.Key("textureFormat");
	writer.Int(m_parameters.textureFormat);

	writer.Key("srvFormat");
	writer.Int(m_parameters.shaderResourceViewFormat);

	writer.Key("dsvFormat");
	writer.Int(m_parameters.depthStencilViewFormat);
}

bool Texture::loadFromFile()
{
	HRESULT hr = S_OK;
	std::wstring wideFilePath = std::wstring(m_filepath.begin(), m_filepath.end());

	ID3D11Resource* resource;
	hr = CreateWICTextureFromFile(m_device, m_context, wideFilePath.c_str(), &resource, &m_textureSRV);
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

	ID3D11Texture2D* texture;
	resource->QueryInterface<ID3D11Texture2D>(&texture);

	resource->Release();

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	m_width = desc.Width;
	m_height = desc.Height;

	texture->Release();
	
	return true;
}

void Texture::createEmpty(TextureParameters parameters)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = parameters.mipLevels;
	desc.ArraySize = 1;
	desc.Format = parameters.textureFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = parameters.usage;
	desc.BindFlags = parameters.bindFlags;

	ID3D11Texture2D* texture;
	HRESULT hr = S_OK;
	m_device->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr))
	{
		Debug::error("Failed to create texture resource for ID " + m_assetID + ".");
		return;
	}

	if ((parameters.bindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = parameters.shaderResourceViewFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;

		hr = m_device->CreateShaderResourceView(texture, &srvDesc, &m_textureSRV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture shader resource view for ID " + m_assetID + ".");
			texture->Release();
			return;
		}
	}

	if ((parameters.bindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = parameters.depthStencilViewFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = m_device->CreateDepthStencilView(texture, &dsvDesc, &m_textureDSV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture depth stencil view for ID " + m_assetID + ".");
			texture->Release();
			return;
		}
	}

	texture->Release();
}

void Texture::createSolidColor(unsigned int hexColor, TextureParameters parameters)
{
	unsigned int color = hexColor;

	// Creates the texture resource
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = parameters.mipLevels;
	desc.ArraySize = 1;
	desc.Format = parameters.textureFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = parameters.usage;
	desc.BindFlags = parameters.bindFlags;

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

	if ((parameters.bindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
	{
		// Creates the texture shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = parameters.shaderResourceViewFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;

		hr = m_device->CreateShaderResourceView(texture, &srvDesc, &m_textureSRV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture shader resource view for ID " + m_assetID + ".");
			texture->Release();
			return;
		}
	}

	if ((parameters.bindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = parameters.depthStencilViewFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = m_device->CreateDepthStencilView(texture, &dsvDesc, &m_textureDSV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture depth stencil view for ID " + m_assetID + ".");
			texture->Release();
			return;
		}
	}

	texture->Release();
}

ID3D11ShaderResourceView* Texture::getSRV() const
{
	return m_textureSRV;
}

ID3D11DepthStencilView* Texture::getDSV() const
{
	return m_textureDSV;
}

unsigned int Texture::getWidth() const
{
	return m_width;
}

unsigned int Texture::getHeight() const
{
	return m_height;
}
