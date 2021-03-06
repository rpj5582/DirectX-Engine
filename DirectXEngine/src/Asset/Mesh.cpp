#include "Mesh.h"

#include "../Debug/Debug.h"
#include <vector>
#include <fstream>

using namespace DirectX;

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath) : Asset(device, context, assetID, filepath)
{
	m_vertices = nullptr;
	m_vertexCount = 0;
	m_indices = nullptr;
	m_indexCount = 0;
}

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID) : Asset(device, context, assetID, "")
{
	m_vertices = nullptr;
	m_vertexCount = 0;
	m_indices = nullptr;
	m_indexCount = 0;
}

Mesh::~Mesh()
{
	if (m_vertexBuffer) { m_vertexBuffer->Release(); }
	if (m_indexBuffer) { m_indexBuffer->Release(); }

	if (m_vertices) delete[] m_vertices;
	if (m_indices) delete[] m_indices;
}

bool Mesh::create(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount, bool immutable)
{
	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

	m_vertices = new Vertex[m_vertexCount];
	m_indices = new unsigned int[m_indexCount];

	memcpy_s(m_vertices, sizeof(Vertex) * m_vertexCount, vertices, sizeof(Vertex) * m_vertexCount);
	memcpy_s(m_indices, sizeof(unsigned int) * m_indexCount, indices, sizeof(unsigned int) * m_indexCount);

	return createBuffers(immutable);
}

void Mesh::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Asset::saveToJSON(writer);

	writer.Key("type");
	writer.String("model");
}

void Mesh::updateVertices() const
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy_s(resource.pData, sizeof(Vertex) * m_vertexCount, m_vertices, sizeof(Vertex) * m_vertexCount);
	m_context->Unmap(m_vertexBuffer, 0);
}

bool Mesh::loadFromFile()
{
	// File input object
	std::ifstream obj(m_filepath, std::ios::in);

	// Check for successful open
	if (!obj.is_open())
	{
		char errorMessage[512];
		strerror_s(errorMessage, 512, errno);
		std::string errString = std::string(errorMessage);
		Debug::warning("Failed to load file from " + m_filepath + " when creating mesh with ID " + m_assetID + ": " + errString);
		return false;
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

	if (vertices.size() > 0 && indices.size() > 0)
	{
		m_vertices = &vertices[0];
		m_indices = &indices[0];

		m_vertexCount = vertices.size();
		m_indexCount = indices.size();

		m_vertices = new Vertex[m_vertexCount];
		m_indices = new unsigned int[m_indexCount];

		memcpy_s(m_vertices, sizeof(Vertex) * m_vertexCount, &vertices[0], sizeof(Vertex) * m_vertexCount);
		memcpy_s(m_indices, sizeof(unsigned int) * m_indexCount, &indices[0], sizeof(unsigned int) * m_indexCount);

		return createBuffers(true);
	}

	return false;
}

bool Mesh::createBuffers(bool immutable)
{

	if (m_vertexCount <= 0 || m_indexCount <= 0)
	{
		Debug::warning("Invalid mesh " + m_assetID + " with no vertices and/or indices.");
		return false;
	}

	calculateTangentsAndBarycentric();

	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = (UINT)(sizeof(Vertex) * m_vertexCount); // number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = m_vertices;

	HRESULT hr = S_OK;

	// Actually create the buffer with the initial data
	hr = m_device->CreateBuffer(&vbd, &initialVertexData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		m_vertices = nullptr;
		m_indices = nullptr;
		Debug::error("Failed to create vertex buffer for mesh " + m_assetID + ".");
		return false;
	}

	// Create the INDEX BUFFER description ------------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = (UINT)(sizeof(UINT) * m_indexCount); // number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = m_indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	hr = m_device->CreateBuffer(&ibd, &initialIndexData, &m_indexBuffer);
	if (FAILED(hr))
	{
		m_indices = nullptr;
		Debug::error("Failed to create index buffer for mesh " + m_assetID + ".");
		return false;
	}

	return true;
}

ID3D11Buffer* Mesh::getVertexBuffer() const
{
	return m_vertexBuffer;
}

ID3D11Buffer* Mesh::getIndexBuffer() const
{
	return m_indexBuffer;
}

Vertex* const Mesh::getVertices()
{
	return m_vertices;
}

unsigned int* const Mesh::getIndices() const
{
	return m_indices;
}

unsigned int Mesh::getVertexCount() const
{
	return m_vertexCount;
}

size_t Mesh::getIndexCount() const
{
	return m_indexCount;
}

// Code adapted from: http://www.terathon.com/code/tangent.html
void Mesh::calculateTangentsAndBarycentric()
{
	for (unsigned int i = 0; i < m_vertexCount;)
	{
		unsigned int i1 = m_indices[i++];
		unsigned int i2 = m_indices[i++];
		unsigned int i3 = m_indices[i++];

		Vertex* v1 = &m_vertices[i1];
		Vertex* v2 = &m_vertices[i2];
		Vertex* v3 = &m_vertices[i3];

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
