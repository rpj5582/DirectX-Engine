#include "Sampler.h"

#include "../Debug/Debug.h"

Sampler::Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, const D3D11_SAMPLER_DESC& samplerDesc) : Asset(device, context, assetID, "")
{
	create(samplerDesc);
}

Sampler::Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
}

Sampler::~Sampler()
{
	if (m_sampler)
		m_sampler->Release();
}

void Sampler::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("sampler");

	D3D11_SAMPLER_DESC desc;
	m_sampler->GetDesc(&desc);

	std::string addressUString = "wrap";
	std::string addressVString = "wrap";
	std::string addressWString = "wrap";

	switch (desc.AddressU)
	{
	case D3D11_TEXTURE_ADDRESS_BORDER:
		addressUString = "border";
		break;

	case D3D11_TEXTURE_ADDRESS_CLAMP:
		addressUString = "clamp";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR:
		addressUString = "mirror";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
		addressUString = "mirror_once";
		break;

	default:
		break;
	}

	switch (desc.AddressV)
	{
	case D3D11_TEXTURE_ADDRESS_BORDER:
		addressVString = "border";
		break;

	case D3D11_TEXTURE_ADDRESS_CLAMP:
		addressVString = "clamp";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR:
		addressVString = "mirror";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
		addressVString = "mirror_once";
		break;

	default:
		break;
	}

	switch (desc.AddressW)
	{
	case D3D11_TEXTURE_ADDRESS_BORDER:
		addressWString = "border";
		break;

	case D3D11_TEXTURE_ADDRESS_CLAMP:
		addressWString = "clamp";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR:
		addressWString = "mirror";
		break;

	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
		addressWString = "mirror_once";
		break;

	default:
		break;
	}

	writer.Key("addressU");
	writer.String(addressUString.c_str());

	writer.Key("addressV");
	writer.String(addressVString.c_str());

	writer.Key("addressW");
	writer.String(addressWString.c_str());

	writer.Key("filter");
	writer.String("min_mag_mip_linear"); // Only this filter type is supported right now

	writer.Key("maxLOD");
	if (desc.MaxLOD == D3D11_FLOAT32_MAX)
	{
		writer.String("float_max");
	}
	else
	{
		writer.Double((double)desc.MaxLOD);
	}
}

bool Sampler::loadAsset()
{
	// Not yet implemented
	Debug::error("Loading samplers from file is not yet implemented.");
	return false;
}

ID3D11SamplerState* Sampler::getSampler() const
{
	return m_sampler;
}

void Sampler::create(const D3D11_SAMPLER_DESC& samplerDesc)
{
	HRESULT hr = S_OK;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_sampler);
	if (FAILED(hr))
	{
		char* errorMsg;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMsg, 0, NULL) != 0)
		{
			std::string errorString = std::string(errorMsg);
			errorString.pop_back();
			errorString.pop_back();
			Debug::warning("Failed to create sampler with ID " + m_assetID + ": " + errorString);
		}
		else
			Debug::warning("Failed to create sampler with ID " + m_assetID + ": Unable to find error description.");
		return;
	}
}
