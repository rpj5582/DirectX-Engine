#include "AssetManager.h"

#include <fstream>
#include <vector>
#include <string>

using namespace DirectX;

AssetManager::AssetManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;

	m_meshes = std::unordered_map<const char*, Mesh*>();
	m_materials = std::unordered_map<const char*, Material*>();
	m_vertexShaders = std::unordered_map<const char*, SimpleVertexShader*>();
	m_pixelShaders = std::unordered_map<const char*, SimplePixelShader*>();
}

AssetManager::~AssetManager()
{
	for (auto it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		delete it->second;
	}
	m_meshes.clear();

	for (auto it = m_materials.begin(); it != m_materials.end(); it++)
	{
		delete it->second;
	}
	m_materials.clear();

	for (auto it = m_vertexShaders.begin(); it != m_vertexShaders.end(); it++)
	{
		delete it->second;
	}
	m_vertexShaders.clear();

	for (auto it = m_pixelShaders.begin(); it != m_pixelShaders.end(); it++)
	{
		delete it->second;
	}
	m_pixelShaders.clear();
}

Mesh* AssetManager::addMesh(const char* id, const char* filename)
{

	std::string fileName = "Assets/" + std::string(filename);
	// File input object
	std::ifstream obj(fileName);

	// Check for successful open
	if (!obj.is_open())
	{
		printf("Failed to load file %s when creating mesh %s.", filename, id);
		return nullptr;
	}

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> vertices;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
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

			Vertex v2;
			v2.position = positions[i[3] - 1];
			v2.uv = uvs[i[4] - 1];
			v2.normal = normals[i[5] - 1];

			Vertex v3;
			v3.position = positions[i[6] - 1];
			v3.uv = uvs[i[7] - 1];
			v3.normal = normals[i[8] - 1];

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

	return addMesh(id, &vertices[0], vertices.size(), &indices[0], indices.size());
}

Mesh* AssetManager::addMesh(const char* id, Vertex* vertices, int vertexCount, UINT* indices, int indexCount)
{
	if (m_meshes.find(id) != m_meshes.end())
	{
		printf("Failed to add mesh %s: Mesh with ID %s already exists", id, id);
		return nullptr;
	}

	Mesh* m = new Mesh(m_device, vertices, vertexCount, indices, indexCount);
	m_meshes[id] = m;

	return m;
}

Material* AssetManager::addMaterial(const char* id, const char* vertexShaderID, const char* pixelShaderID)
{
	if (m_materials.find(id) != m_materials.end())
	{
		printf("Failed to add material %s: Material with ID %s already exists", id, id);
		return nullptr;
	}

	if (m_vertexShaders.find(vertexShaderID) == m_vertexShaders.end())
	{
		printf("Invalid vertex shader ID %s", vertexShaderID);
		return nullptr;
	}

	if (m_pixelShaders.find(pixelShaderID) == m_pixelShaders.end())
	{
		printf("Invalid pixel shader ID %s", pixelShaderID);
		return nullptr;
	}

	Material* m = new Material(m_device, m_context, m_vertexShaders[vertexShaderID], m_pixelShaders[pixelShaderID]);
	m_materials[id] = m;

	return m;
}

void AssetManager::loadShader(const char* id, ShaderType type, LPCWSTR filename)
{
	switch (type)
	{
	case VertexShader:
	{
		if (m_vertexShaders.find(id) != m_vertexShaders.end())
		{
			printf("Failed to load vertex shader %s: Vertex shader with ID %s already exists", id, id);
			return;
		}

		SimpleVertexShader* vs = new SimpleVertexShader(m_device, m_context);
		vs->LoadShaderFile(filename);
		m_vertexShaders[id] = vs;
		break;
	}

	case PixelShader:
	{
		if (m_pixelShaders.find(id) != m_pixelShaders.end())
		{
			printf("Failed to load pixel shader %s: Pixel shader with ID %s already exists", id, id);
			return;
		}

		SimplePixelShader* ps = new SimplePixelShader(m_device, m_context);
		ps->LoadShaderFile((LPCWSTR)filename);
		m_pixelShaders[id] = ps;
		break;
	}
	default:
	{
		printf("Invalid shader type %d", type);
		break;
	}
	}
}
