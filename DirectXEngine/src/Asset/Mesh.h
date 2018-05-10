#pragma once
#include "Asset.h"

#include <d3d11.h>
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 barycentric;
};

class Mesh : public Asset
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, std::string filepath);
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID);
	~Mesh();

	bool create(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount, bool immutable = true);
	bool loadFromFile() override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	void updateVertices() const;

	ID3D11Buffer* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;

	Vertex* const getVertices();
	unsigned int* const getIndices() const;
	unsigned int getVertexCount() const;
	unsigned int getIndexCount() const;

private:
	bool createBuffers(bool immutable);

	// A helper function when loading meshes to calculate tangents for the normal map lighting calculation and assigns barycentric coordinates for solid wireframe rendering.
	void calculateTangentsAndBarycentric();

	Vertex* m_vertices;
	unsigned int m_vertexCount;
	unsigned int* m_indices;
	unsigned int m_indexCount;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
};