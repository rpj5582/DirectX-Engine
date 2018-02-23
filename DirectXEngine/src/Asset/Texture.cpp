#include "Texture.h"

#include "../Debug/Debug.h"
#include "WICTextureLoader.h"

using namespace DirectX;

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID) : Asset(device, context, assetID, "")
{
	m_textureSRV = nullptr;
	m_textureDSV = nullptr;
	m_textureRTV = nullptr;

	m_width = 0;
	m_height = 0;
	m_parameters = {};
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath, TextureParameters parameters)
	: Asset(device, context, assetID, filepath)
{
	m_textureSRV = nullptr;
	m_textureDSV = nullptr;
	m_textureRTV = nullptr;

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

	if (m_textureRTV)
	{
		m_textureRTV->Release();
		m_textureRTV = nullptr;
	}
}

bool Texture::create(unsigned int width, unsigned int height, TextureParameters parameters)
{
	m_width = width;
	m_height = height;
	m_parameters = parameters;
	return createEmpty();
}

bool Texture::create(unsigned int hexColor, TextureParameters parameters)
{
	m_width = 1;
	m_height = 1;
	m_parameters = parameters;
	return createSolidColor(hexColor);
}

void Texture::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("texture");

	writer.Key("width");
	writer.Uint(m_width);
	
	writer.Key("height");
	writer.Uint(m_height);

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
			Debug::warning("Failed to load texture with ID " + m_assetID + " at " + m_filepath + ": " + errorString);
		}
		else
			Debug::warning("Failed to load texture with ID " + m_assetID + " at " + m_filepath + ": Unable to find error description.");

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

bool Texture::createEmpty()
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = m_parameters.mipLevels;
	desc.ArraySize = 1;
	desc.Format = m_parameters.textureFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = m_parameters.usage;
	desc.BindFlags = m_parameters.bindFlags;

	ID3D11Texture2D* texture;
	HRESULT hr = S_OK;
	hr = m_device->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr))
	{
		Debug::error("Failed to create texture resource for ID " + m_assetID + ".");
		return false;
	}

	return createViews(texture);
}

bool Texture::createSolidColor(unsigned int hexColor)
{
	unsigned int color = hexColor;

	// Creates the texture resource
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = m_parameters.mipLevels;
	desc.ArraySize = 1;
	desc.Format = m_parameters.textureFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = m_parameters.usage;
	desc.BindFlags = m_parameters.bindFlags;

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
		return false;
	}

	return createViews(texture);
}

bool Texture::createViews(ID3D11Texture2D* texture)
{
	HRESULT hr = S_OK;

	if ((m_parameters.bindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = m_parameters.shaderResourceViewFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;

		hr = m_device->CreateShaderResourceView(texture, &srvDesc, &m_textureSRV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture shader resource view for ID " + m_assetID + ".");
			texture->Release();
			return false;
		}
	}

	if ((m_parameters.bindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_parameters.depthStencilViewFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = m_device->CreateDepthStencilView(texture, &dsvDesc, &m_textureDSV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture depth stencil view for ID " + m_assetID + ".");
			texture->Release();
			return false;
		}
	}

	if ((m_parameters.bindFlags & D3D11_BIND_RENDER_TARGET) == D3D11_BIND_RENDER_TARGET)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_parameters.renderTargetViewFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		hr = m_device->CreateRenderTargetView(texture, &rtvDesc, &m_textureRTV);
		if (FAILED(hr))
		{
			Debug::error("Failed to create texture render target view for ID " + m_assetID + ".");
			texture->Release();
			return false;
		}
	}

	texture->Release();
	return true;
}

ID3D11ShaderResourceView* Texture::getSRV() const
{
	return m_textureSRV;
}

ID3D11DepthStencilView* Texture::getDSV() const
{
	return m_textureDSV;
}

ID3D11RenderTargetView* Texture::getRTV() const
{
	return m_textureRTV;
}

unsigned int Texture::getWidth() const
{
	return m_width;
}

unsigned int Texture::getHeight() const
{
	return m_height;
}
