#include "AssetManager.h"

#include "WICTextureLoader.h"
#include <fstream>
#include <vector>
#include <string>

using namespace DirectX;

AssetManager* AssetManager::m_instance = nullptr;

AssetManager::AssetManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	if (m_instance == nullptr)
		m_instance = this;
	else
		return;

	m_device = device;
	m_context = context;

	m_meshes = std::unordered_map<std::string, std::pair<std::string, Mesh*>>();
	m_materials = std::unordered_map<std::string, std::pair<std::string, Material*>>();
	m_vertexShaders = std::unordered_map<std::string, std::pair<std::string, SimpleVertexShader*>>();
	m_pixelShaders = std::unordered_map<std::string, std::pair<std::string, SimplePixelShader*>>();
	m_textures = std::unordered_map<std::string, std::pair<std::string, ID3D11ShaderResourceView*>>();
	m_samplers = std::unordered_map<std::string, std::pair<std::string, ID3D11SamplerState*>>();
	m_fonts = std::unordered_map<std::string, std::pair<std::string, SpriteFont*>>();
}

AssetManager::~AssetManager()
{
	for (auto it = m_fonts.begin(); it != m_fonts.end(); it++)
	{
		delete it->second.second;
	}
	m_fonts.clear();

	for (auto it = m_samplers.begin(); it != m_samplers.end(); it++)
	{
		it->second.second->Release();
	}
	m_samplers.clear();

	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		it->second.second->Release();
	}
	m_textures.clear();

	for (auto it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		delete it->second.second;
	}
	m_meshes.clear();

	for (auto it = m_materials.begin(); it != m_materials.end(); it++)
	{
		delete it->second.second;
	}
	m_materials.clear();

	for (auto it = m_vertexShaders.begin(); it != m_vertexShaders.end(); it++)
	{
		delete it->second.second;
	}
	m_vertexShaders.clear();

	for (auto it = m_pixelShaders.begin(); it != m_pixelShaders.end(); it++)
	{
		delete it->second.second;
	}
	m_pixelShaders.clear();
}

bool AssetManager::init()
{
	// Loads the default shaders
	if (!loadShader("default", VertexShader, "VertexShader.cso")) return false;
	if (!loadShader("default", PixelShader, "PixelShader.cso")) return false;

	// Creates the default textures
	if (!createSolidColorTexture("defaultDiffuse", 0xff808080)) return false;
	if (!createSolidColorTexture("defaultSpecular", 0xffffffff)) return false;
	if (!createSolidColorTexture("defaultNormal", 0xffff8080)) return false;
	if (!createSolidColorTexture("defaultGUI", 0xffffffff)) return false;

	// Creates the default sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* defaultSampler = createSampler("default", samplerDesc);
	if (!defaultSampler) return false;

	// Creates the default material
	Material* defaultMaterial = createMaterial("default");
	if (!defaultMaterial) return false;
	
	// Load Arial font
	SpriteFont* arial = loadFont("default", "Arial_16pt.spritefont");

	return true;
}

void AssetManager::loadFromJSON(rapidjson::Value& assetsArray)
{
	for (rapidjson::SizeType i = 0; i < assetsArray.Size(); i++)
	{
		rapidjson::Value& asset = assetsArray[i];
		rapidjson::Value& assetType = asset["type"];

		rapidjson::Value& idRef = asset["id"];
		std::string id = idRef.GetString();

		rapidjson::Value::MemberIterator pathIter = asset.FindMember("path");
		std::string path = "";
		if (pathIter != asset.MemberEnd())
		{
			path = pathIter->value.GetString();
		}

		switch (stringHash(assetType.GetString()))
		{
		case stringHash("texture"):
		{
			loadTexture(id, path);
			break;
		}

		case stringHash("model"):
		{
			loadMesh(id, path);
			break;
		}

		case stringHash("font"):
		{
			loadFont(id, path);
			break;
		}

		case stringHash("vertexShader"):
		{
			loadShader(id, VertexShader, path);
			break;
		}

		case stringHash("pixelShader"):
		{
			loadShader(id, PixelShader, path);
			break;
		}

		case stringHash("material"):
		{
			rapidjson::Value::MemberIterator diffuseIter = asset.FindMember("diffuse");
			rapidjson::Value::MemberIterator specularIter = asset.FindMember("specular");
			rapidjson::Value::MemberIterator normalIter = asset.FindMember("normal");
			rapidjson::Value::MemberIterator samplerIter = asset.FindMember("sampler");
			rapidjson::Value::MemberIterator vertexShaderIter = asset.FindMember("vertexShader");
			rapidjson::Value::MemberIterator pixelShaderIter = asset.FindMember("pixelShader");

			std::string diffuse = "defaultDiffuse";
			std::string specular = "defaultSpecular";
			std::string normal = "defaultNormal";
			std::string sampler = "default";
			std::string vertexShader = "default";
			std::string pixelShader = "default";

			if (diffuseIter != asset.MemberEnd())
			{
				diffuse = diffuseIter->value.GetString();
			}

			if (specularIter != asset.MemberEnd())
			{
				specular = specularIter->value.GetString();
			}

			if (normalIter != asset.MemberEnd())
			{
				normal = normalIter->value.GetString();
			}

			if (samplerIter != asset.MemberEnd())
			{
				sampler = samplerIter->value.GetString();
			}

			if (vertexShaderIter != asset.MemberEnd())
			{
				vertexShader = vertexShaderIter->value.GetString();
			}

			if (pixelShaderIter != asset.MemberEnd())
			{
				pixelShader = pixelShaderIter->value.GetString();
			}

			createMaterial(id, diffuse, specular, normal, sampler, vertexShader, pixelShader);
			break;
		}

		case stringHash("sampler"):
		{
			rapidjson::Value::MemberIterator addressUIter = asset.FindMember("addressU");
			rapidjson::Value::MemberIterator addressVIter = asset.FindMember("addressV");
			rapidjson::Value::MemberIterator addressWIter = asset.FindMember("addressW");
			rapidjson::Value::MemberIterator filterIter = asset.FindMember("filter");
			rapidjson::Value::MemberIterator maxLODIter = asset.FindMember("maxLOD");

			std::string addressUString = "wrap";
			std::string addressVString = "wrap";
			std::string addressWString = "wrap";
			std::string filterString = "min_mag_mip_linear";
			float maxLOD = D3D11_FLOAT32_MAX;

			if (addressUIter != asset.MemberEnd())
			{
				addressUString = addressUIter->value.GetString();
			}

			if (addressVIter != asset.MemberEnd())
			{
				addressVString = addressVIter->value.GetString();
			}

			if (addressWIter != asset.MemberEnd())
			{
				addressWString = addressWIter->value.GetString();
			}

			if (filterIter != asset.MemberEnd())
			{
				filterString = filterIter->value.GetString();
			}

			if (maxLODIter != asset.MemberEnd())
			{
				if(maxLODIter->value.IsFloat())
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
				Output::Warning("Invalid sampler address mode " + addressUString + " for sampler " + id);
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
				Output::Warning("Invalid sampler address mode " + addressVString + " for sampler " + id);
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
				Output::Warning("Invalid sampler address mode " + addressWString + " for sampler " + id);
				break;
			}

			// Don't support different sampler filters yet because there are too many options


			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.AddressU = addressU;
			samplerDesc.AddressV = addressV;
			samplerDesc.AddressW = addressW;
			samplerDesc.Filter = filter;
			samplerDesc.MaxLOD = maxLOD;

			createSampler(id, samplerDesc);
			break;
		}

		default:
			Output::Warning("Invalid asset type " + std::string(assetType.GetString()) + ", skipping.");
			break;
		}
	}
}

void AssetManager::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	for (auto it = m_instance->m_textures.begin(); it != m_instance->m_textures.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save generated textures
		if (assetPath.empty()) continue;

		writer.StartObject();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("texture");

		writer.EndObject();
	}

	for (auto it = m_instance->m_meshes.begin(); it != m_instance->m_meshes.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save generated meshes
		if (assetPath.empty()) continue;

		writer.StartObject();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("model");

		writer.EndObject();
	}

	for (auto it = m_instance->m_vertexShaders.begin(); it != m_instance->m_vertexShaders.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save default or generated shaders
		if (assetID == "default" || assetPath.empty()) continue;

		writer.StartObject();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("vertexShader");

		writer.EndObject();
	}

	for (auto it = m_instance->m_pixelShaders.begin(); it != m_instance->m_pixelShaders.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save default or generated shaders
		if (assetID == "default" || assetPath.empty()) continue;

		writer.StartObject();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("pixelShader");

		writer.EndObject();
	}

	for (auto it = m_instance->m_fonts.begin(); it != m_instance->m_fonts.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save default or generated fonts
		if (assetID == "default" || assetPath.empty()) continue;

		writer.StartObject();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("font");

		writer.EndObject();
	}

	for (auto it = m_instance->m_materials.begin(); it != m_instance->m_materials.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save the default material
		if (assetID == "default") continue;

		writer.StartObject();

		std::string vertexShaderID = it->second.second->getVertexShaderID();
		std::string pixelShaderID = it->second.second->getPixelShaderID();
		std::string diffuseID = it->second.second->getDiffuseTextureID();
		std::string specularID = it->second.second->getSpecularTextureID();
		std::string normalID = it->second.second->getNormalTextureID();
		std::string samplerID = it->second.second->getSamplerID();

		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("material");

		writer.Key("vertexShader");
		writer.String(vertexShaderID.c_str());

		writer.Key("pixelShader");
		writer.String(pixelShaderID.c_str());

		writer.Key("diffuse");
		writer.String(diffuseID.c_str());

		writer.Key("specular");
		writer.String(specularID.c_str());

		writer.Key("normal");
		writer.String(normalID.c_str());

		writer.Key("sampler");
		writer.String(samplerID.c_str());

		writer.EndObject();
	}

	for (auto it = m_instance->m_samplers.begin(); it != m_instance->m_samplers.end(); it++)
	{
		std::string assetID = it->first;
		std::string assetPath = it->second.first;

		// Don't save default or generated samplers
		if (assetID == "default" || assetPath.empty()) continue;

		writer.StartObject();
		
		writer.Key("id");
		writer.String(assetID.c_str());

		writer.Key("path");
		writer.String(assetPath.c_str());

		writer.Key("type");
		writer.String("sampler");

		D3D11_SAMPLER_DESC desc;
		it->second.second->GetDesc(&desc);

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

		writer.EndObject();
	}
}

Mesh* AssetManager::getMesh(std::string id)
{
	if (m_instance->m_meshes.find(id) == m_instance->m_meshes.end())
	{
		Output::Error("Could not find mesh with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_meshes.at(id).second;
}

Material* AssetManager::getMaterial(std::string id)
{
	if (m_instance->m_materials.find(id) == m_instance->m_materials.end())
	{
		Output::Error("Could not find material with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_materials.at(id).second;
}

ID3D11ShaderResourceView* AssetManager::getTexture(std::string id)
{
	if (m_instance->m_textures.find(id) == m_instance->m_textures.end())
	{
		Output::Error("Could not find texture with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_textures.at(id).second;
}

ID3D11SamplerState* AssetManager::getSampler(std::string id)
{
	if (m_instance->m_samplers.find(id) == m_instance->m_samplers.end())
	{
		Output::Error("Could not find sampler with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_samplers.at(id).second;
}

SimpleVertexShader* AssetManager::getVertexShader(std::string id)
{
	if (m_instance->m_vertexShaders.find(id) == m_instance->m_vertexShaders.end())
	{
		Output::Error("Could not find vertex shader with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_vertexShaders.at(id).second;
}

SimplePixelShader* AssetManager::getPixelShader(std::string id)
{
	if (m_instance->m_pixelShaders.find(id) == m_instance->m_pixelShaders.end())
	{
		Output::Error("Could not find pixel shader with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_pixelShaders.at(id).second;
}

SpriteFont* AssetManager::getFont(std::string id)
{
	if (m_instance->m_fonts.find(id) == m_instance->m_fonts.end())
	{
		Output::Error("Could not find font with ID " + id + ".");
		return nullptr;
	}

	return m_instance->m_fonts.at(id).second;
}

Mesh* AssetManager::loadMesh(std::string id, std::string filepath)
{
	std::string filePathStr = "Assets/Models/" + filepath;
	// File input object
	std::ifstream obj(filePathStr, std::ios::in);

	// Check for successful open
	if (!obj.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Output::Error("Failed to load file from " + filePathStr + " when creating mesh with ID " + id + ": " + errString);
		return nullptr;
	}

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> vertices;           // Verts we're assembling
	std::vector<unsigned int> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];                     // String for line reading

	while (obj.good()) // Still have data left?
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.position = positions[i[0] - 1];
			v1.uv = uvs[i[1] - 1];
			v1.normal = normals[i[2] - 1];
			v1.tangent = XMFLOAT3();

			Vertex v2;
			v2.position = positions[i[3] - 1];
			v2.uv = uvs[i[4] - 1];
			v2.normal = normals[i[5] - 1];
			v2.tangent = XMFLOAT3();

			Vertex v3;
			v3.position = positions[i[6] - 1];
			v3.uv = uvs[i[7] - 1];
			v3.normal = normals[i[8] - 1];
			v3.tangent = XMFLOAT3();

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.uv.y = 1.0f - v1.uv.y;
			v2.uv.y = 1.0f - v2.uv.y;
			v3.uv.y = 1.0f - v3.uv.y;

			// Flip Z (LH vs. RH)
			v1.position.z *= -1.0f;
			v2.position.z *= -1.0f;
			v3.position.z *= -1.0f;

			// Flip normal Z
			v1.normal.z *= -1.0f;
			v2.normal.z *= -1.0f;
			v3.normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			vertices.push_back(v1);
			vertices.push_back(v3);
			vertices.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4;
				v4.position = positions[i[9] - 1];
				v4.uv = uvs[i[10] - 1];
				v4.normal = normals[i[11] - 1];
				v4.tangent = XMFLOAT3();

				// Flip the UV, Z pos and normal
				v4.uv.y = 1.0f - v4.uv.y;
				v4.position.z *= -1.0f;
				v4.normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				vertices.push_back(v1);
				vertices.push_back(v4);
				vertices.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();

	return loadMesh(id, filepath, &vertices[0], vertices.size(), &indices[0], indices.size());
}

Mesh* AssetManager::loadMesh(std::string id, std::string filepath, Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount)
{
	auto meshIterator = m_instance->m_meshes.find(id);
	if (meshIterator != m_instance->m_meshes.end())
	{
		Output::Warning("Mesh with ID " + id + " already exists, consider getting the mesh instead of loading it again.");
		return meshIterator->second.second;
	}

	m_instance->calculateTangentsAndBarycentric(vertices, vertexCount, indices);

	Mesh* m = new Mesh(m_instance->m_device, vertices, vertexCount, indices, indexCount);
	m_instance->m_meshes[id] = std::pair<std::string, Mesh*>(filepath, m);

	return m;
}

// Code adapted from: http://www.terathon.com/code/tangent.html
void AssetManager::calculateTangentsAndBarycentric(Vertex* vertices, unsigned int vertexCount, unsigned int* indices)
{
	for (unsigned int i = 0; i < vertexCount;)
	{
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];

		Vertex* v1 = &vertices[i1];
		Vertex* v2 = &vertices[i2];
		Vertex* v3 = &vertices[i3];

		float x1 = v2->position.x - v1->position.x;
		float y1 = v2->position.y - v1->position.y;
		float z1 = v2->position.z - v1->position.z;

		float x2 = v3->position.x - v1->position.x;
		float y2 = v3->position.y - v1->position.y;
		float z2 = v3->position.z - v1->position.z;

		float s1 = v2->uv.x - v1->uv.x;
		float t1 = v2->uv.y - v1->uv.y;

		float s2 = v3->uv.x - v1->uv.x;
		float t2 = v3->uv.y - v1->uv.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		v1->tangent.x += tx;
		v1->tangent.y += ty;
		v1->tangent.z += tz;

		v2->tangent.x += tx;
		v2->tangent.y += ty;
		v2->tangent.z += tz;

		v3->tangent.x += tx;
		v3->tangent.y += ty;
		v3->tangent.z += tz;

		v1->barycentric = XMFLOAT3(1.0f, 0.0f, 0.0f);
		v2->barycentric = XMFLOAT3(0.0f, 1.0f, 0.0f);
		v3->barycentric = XMFLOAT3(0.0f, 0.0f, 1.0f);
	}
}

Material* AssetManager::createMaterial(std::string id)
{
	return createMaterial(id, "defaultDiffuse");
}

Material* AssetManager::createMaterial(std::string id, std::string diffuseTextureID)
{
	return createMaterial(id, diffuseTextureID, "defaultSpecular");
}

Material* AssetManager::createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID)
{
	return createMaterial(id, diffuseTextureID, specularTextureID, "defaultNormal");
}

Material* AssetManager::createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID)
{
	return createMaterial(id, diffuseTextureID, specularTextureID, normalTextureID, "default");
}

Material* AssetManager::createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID, std::string samplerID)
{
	return createMaterial(id, diffuseTextureID, specularTextureID, normalTextureID, samplerID, "default", "default");
}

Material* AssetManager::createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID, std::string samplerID, std::string vertexShaderID, std::string pixelShaderID)
{
	auto materialIterator = m_instance->m_materials.find(id);
	if (materialIterator != m_instance->m_materials.end())
	{
		Output::Warning("Material with ID " + id + " already exists, consider getting the material instead of loading it again.");
		return materialIterator->second.second;
	}

	if (m_instance->m_vertexShaders.find(vertexShaderID) == m_instance->m_vertexShaders.end())
	{
		Output::Error("Invalid vertex shader ID " + vertexShaderID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	if (m_instance->m_pixelShaders.find(pixelShaderID) == m_instance->m_pixelShaders.end())
	{
		Output::Error("Invalid pixel shader ID " + pixelShaderID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	if (m_instance->m_textures.find(diffuseTextureID) == m_instance->m_textures.end())
	{
		Output::Error("Invalid diffuse texture ID " + diffuseTextureID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	if (m_instance->m_textures.find(specularTextureID) == m_instance->m_textures.end())
	{
		Output::Error("Invalid specular texture ID " + specularTextureID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	if (m_instance->m_textures.find(normalTextureID) == m_instance->m_textures.end())
	{
		Output::Error("Invalid normal texture ID " + normalTextureID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	if (m_instance->m_samplers.find(samplerID) == m_instance->m_samplers.end())
	{
		Output::Error("Invalid sampler ID " + samplerID + " when creating material with ID " + id + ".");
		return nullptr;
	}

	Material* m = new Material(vertexShaderID, pixelShaderID, diffuseTextureID, specularTextureID, normalTextureID, samplerID);

	m_instance->m_materials[id] = std::pair<std::string, Material*>("", m);
	return m;
}

bool AssetManager::loadShader(std::string id, ShaderType type, std::string filepath)
{
	switch (type)
	{
		case VertexShader:
		{
			auto vertexInterator = m_instance->m_vertexShaders.find(id);
			if (vertexInterator != m_instance->m_vertexShaders.end())
			{
				Output::Warning("Vertex shader with ID " + id + " already exists, consider getting the vertex shader instead of loading it again.");
				return vertexInterator->second.second;
			}

			SimpleVertexShader* vs = new SimpleVertexShader(m_instance->m_device, m_instance->m_context);
			std::wstring filePathStr = L"Assets/Shaders/" + std::wstring(filepath.begin(), filepath.end());
			if (!vs->LoadShaderFile(filePathStr.c_str()))
			{
				Output::Error("Failed to load vertex shader with ID " + id + " at file location " + std::string(filePathStr.begin(), filePathStr.end()));
				delete vs;
				return false;
			}

			m_instance->m_vertexShaders[id] = std::pair<std::string, SimpleVertexShader*>(filepath, vs);
			break;
		}

		case PixelShader:
		{
			auto pixelIterator = m_instance->m_pixelShaders.find(id);
			if (pixelIterator != m_instance->m_pixelShaders.end())
			{
				Output::Warning("Pixel shader with ID " + id + " already exists, consider getting the pixel shader instead of loading it again.");
				return pixelIterator->second.second;
			}

			std::wstring filePathStr = L"Assets/Shaders/" + std::wstring(filepath.begin(), filepath.end());
			SimplePixelShader* ps = new SimplePixelShader(m_instance->m_device, m_instance->m_context);
			if (!ps->LoadShaderFile(filePathStr.c_str()))
			{
				Output::Error("Failed to load pixel shader with ID " + id + " at file location " + std::string(filePathStr.begin(), filePathStr.end()));
				delete ps;
				return false;
			}
			m_instance->m_pixelShaders[id] = std::pair<std::string, SimplePixelShader*>(filepath, ps);
			break;
		}
		default:
		{
			Output::Error("Invalid shader type " + std::to_string(type) + ".");
			return false;
		}
	}

	return true;
}

ID3D11ShaderResourceView* AssetManager::loadTexture(std::string id, std::string filepath)
{
	// Return a pointer to the texture if it has already been loaded
	auto textureIterator = m_instance->m_textures.find(id);
	if (textureIterator != m_instance->m_textures.end())
	{
		Output::Warning("Texture with ID " + id + " already exists, consider getting the texture instead of loading it again.");
		return textureIterator->second.second;
	}
	
	ID3D11ShaderResourceView* textureSRV;
	HRESULT hr = S_OK;
	std::wstring filePathStr = L"Assets/Textures/" + std::wstring(filepath.begin(), filepath.end());
	hr = CreateWICTextureFromFile(m_instance->m_device, m_instance->m_context, filePathStr.c_str(), nullptr, &textureSRV);
	if (FAILED(hr) || !textureSRV)
	{
		char* errorMsg;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMsg, 0, NULL) != 0)
		{
			std::string errorString = std::string(errorMsg);
			errorString.pop_back();
			errorString.pop_back();
			Output::Error("Failed to load texture with ID " + id + " at Assets/Textures/" + filepath + ": " + errorString);
		}
		else
			Output::Error("Failed to load texture with ID " + id + " at Assets/Textures/" + filepath + ": Unable to find error description.");
		
		return nullptr;
	}

	m_instance->m_textures[id] = std::pair<std::string, ID3D11ShaderResourceView*>(filepath, textureSRV);
	return textureSRV;
}

ID3D11ShaderResourceView* AssetManager::createSolidColorTexture(std::string id, unsigned int hexColor)
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
	m_instance->m_device->CreateTexture2D(&desc, &pixelData, &texture);
	if (FAILED(hr))
	{
		Output::Error("Failed to create texture resource for ID " + id + ".");
		return nullptr;
	}

	// Creates the texture shader resource view
	D3D11_TEX2D_SRV srvDescTexture2D;
	srvDescTexture2D.MipLevels = -1;
	srvDescTexture2D.MostDetailedMip = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D = srvDescTexture2D;

	ID3D11ShaderResourceView* textureSRV;
	hr = m_instance->m_device->CreateShaderResourceView(texture, &srvDesc, &textureSRV);
	if (FAILED(hr))
	{
		Output::Error("Failed to create texture shader resource view for ID " + id + ".");
		texture->Release();
		return nullptr;
	}

	m_instance->m_textures[id] = std::pair<std::string, ID3D11ShaderResourceView*>("", textureSRV);
	texture->Release();

	return textureSRV;
}

DirectX::SpriteFont* AssetManager::loadFont(std::string id, std::string filepath)
{
	std::wstring filePath = L"Assets/Fonts/" + std::wstring(filepath.begin(), filepath.end());

	// Return a pointer to the font if it has already been loaded
	auto fontIterator = m_instance->m_fonts.find(id);
	if (fontIterator != m_instance->m_fonts.end())
	{
		Output::Warning("Font with ID " + id + " already exists, consider getting the font instead of loading it again.");
		return fontIterator->second.second;
	}

	SpriteFont* font = new SpriteFont(m_instance->m_device, filePath.c_str());
	if (!font)
	{
		Output::Error("Failed to load font " + id + " at Assets/Fonts/" + filepath);
		return nullptr;
	}

	m_instance->m_fonts[id] = std::pair<std::string, SpriteFont*>(filepath, font);
	return font;
}

ID3D11SamplerState* AssetManager::createSampler(std::string id, const D3D11_SAMPLER_DESC& samplerDesc)
{
	auto samplerIterator = m_instance->m_samplers.find(id);
	if (samplerIterator != m_instance->m_samplers.end())
	{
		Output::Warning("Sampler with ID " + id + " already exists, consider getting the sampler instead of creating it again.");
		return samplerIterator->second.second;
	}

	ID3D11SamplerState* sampler;

	HRESULT hr = S_OK;
	hr = m_instance->m_device->CreateSamplerState(&samplerDesc, &sampler);
	if (FAILED(hr))
	{
		char* errorMsg;
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMsg, 0, NULL) != 0)
		{
			std::string errorString = std::string(errorMsg);
			errorString.pop_back();
			errorString.pop_back();
			Output::Error("Failed to create sampler with ID " + id + ": " + errorString);
		}
		else
			Output::Error("Failed to create sampler with ID " + id + ": Unable to find error description.");
		return nullptr;
	}

	m_instance->m_samplers[id] = std::pair<std::string, ID3D11SamplerState*>("", sampler);
	return sampler;
}
