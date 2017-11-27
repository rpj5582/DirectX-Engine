#include "Sampler.h"

#include "../Debug/Debug.h"

#include "rapidjson/error/en.h"
#include "../Util.h"
#include <fstream>

Sampler::Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID) : Asset(device, context, assetID, "")
{
	m_samplerDesc = {};
}

Sampler::Sampler(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
	m_samplerDesc = {};
}

Sampler::~Sampler()
{
	if (m_sampler)
		m_sampler->Release();
}

bool Sampler::create(const D3D11_SAMPLER_DESC& samplerDesc)
{
	m_samplerDesc = samplerDesc;

	HRESULT hr = S_OK;
	hr = m_device->CreateSamplerState(&m_samplerDesc, &m_sampler);
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
		return false;
	}

	return true;
}

void Sampler::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("sampler");

	std::string addressUString = "wrap";
	std::string addressVString = "wrap";
	std::string addressWString = "wrap";

	switch (m_samplerDesc.AddressU)
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

	switch (m_samplerDesc.AddressV)
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

	switch (m_samplerDesc.AddressW)
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
	if (m_samplerDesc.MaxLOD == D3D11_FLOAT32_MAX)
	{
		writer.String("float_max");
	}
	else
	{
		writer.Double(m_samplerDesc.MaxLOD);
	}
}

bool Sampler::loadFromFile()
{
	// Load the json file
	std::ifstream ifs(m_filepath, std::ios::in | std::ios::binary);

	if (!ifs.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::warning("Failed to load file from " + m_filepath + " when creating sampler with ID " + m_assetID + ": " + errString);
		return false;
	}

	unsigned int length;
	ifs.seekg(0, std::ios::end);
	length = (unsigned int)ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	char* jsonStringBuffer = new char[length + 1];
	ifs.read(jsonStringBuffer, length);
	jsonStringBuffer[length] = '\0';

	rapidjson::Document dom;
	rapidjson::ParseResult result = dom.Parse(jsonStringBuffer);
	if (result.IsError())
	{
		const char* errorMessage = rapidjson::GetParseError_En(result.Code());

		Debug::error("Failed to load sampler at " + m_filepath + " because there was a parse error at character " + std::to_string(result.Offset()) + ": " + std::string(errorMessage));
		delete[] jsonStringBuffer;
		return false;
	}

	delete[] jsonStringBuffer;

	ifs.close();

	rapidjson::Value::MemberIterator addressUIter = dom.FindMember("addressU");
	rapidjson::Value::MemberIterator addressVIter = dom.FindMember("addressV");
	rapidjson::Value::MemberIterator addressWIter = dom.FindMember("addressW");
	rapidjson::Value::MemberIterator filterIter = dom.FindMember("filter");
	rapidjson::Value::MemberIterator maxLODIter = dom.FindMember("maxLOD");

	std::string addressUString = "wrap";
	std::string addressVString = "wrap";
	std::string addressWString = "wrap";
	std::string filterString = "min_mag_mip_linear";
	float maxLOD = D3D11_FLOAT32_MAX;

	if (addressUIter != dom.MemberEnd())
	{
		addressUString = addressUIter->value.GetString();
	}

	if (addressVIter != dom.MemberEnd())
	{
		addressVString = addressVIter->value.GetString();
	}

	if (addressWIter != dom.MemberEnd())
	{
		addressWString = addressWIter->value.GetString();
	}

	if (filterIter != dom.MemberEnd())
	{
		filterString = filterIter->value.GetString();
	}

	if (maxLODIter != dom.MemberEnd())
	{
		if (maxLODIter->value.IsFloat())
			maxLOD = maxLODIter->value.GetFloat();
		else
		{
			maxLOD = D3D11_FLOAT32_MAX;
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	switch (stringHash(addressUString.c_str()))
	{
	case stringHash("wrap"):
		break;

	case stringHash("border"):
		addressU = D3D11_TEXTURE_ADDRESS_BORDER;
		break;

	case stringHash("clamp"):
		addressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;

	case stringHash("mirror"):
		addressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;

	case stringHash("mirror_once"):
		addressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;

	default:
		Debug::warning("Invalid sampler address mode " + addressUString + " for sampler " + m_assetID);
		break;
	}

	switch (stringHash(addressVString.c_str()))
	{
	case stringHash("wrap"):
		break;

	case stringHash("border"):
		addressV = D3D11_TEXTURE_ADDRESS_BORDER;
		break;

	case stringHash("clamp"):
		addressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;

	case stringHash("mirror"):
		addressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;

	case stringHash("mirror_once"):
		addressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;

	default:
		Debug::warning("Invalid sampler address mode " + addressVString + " for sampler " + m_assetID);
		break;
	}

	switch (stringHash(addressWString.c_str()))
	{
	case stringHash("wrap"):
		break;

	case stringHash("border"):
		addressW = D3D11_TEXTURE_ADDRESS_BORDER;
		break;

	case stringHash("clamp"):
		addressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;

	case stringHash("mirror"):
		addressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;

	case stringHash("mirror_once"):
		addressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;

	default:
		Debug::warning("Invalid sampler address mode " + addressWString + " for sampler " + m_assetID);
		break;
	}

	// Don't support different sampler filters yet because there are too many options

	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = addressU;
	desc.AddressV = addressV;
	desc.AddressW = addressW;
	desc.Filter = filter;
	desc.MaxLOD = maxLOD;

	return create(desc);
}

ID3D11SamplerState* Sampler::getSamplerState() const
{
	return m_sampler;
}
