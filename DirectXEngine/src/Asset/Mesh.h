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
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context, std::string assetID, Vertex* vertices, unsigned int vertexCount, UINT* indices, unsigned int indexCount);
	~Mesh();

	bool loadAsset() override;
	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	ID3D11Buffer* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;

	unsigned int getIndexCount() const;

private:
	void createBuffers(ID3D11Device* device, Vertex* vertices, unsigned int vertexCount, UINT* indices, unsigned int indexCount);

	// A helper function when loading meshes to calculate tangents for the normal map lighting calculation and assigns barycentric coordinates for solid wireframe rendering.
	void calculateTangentsAndBarycentric(Vertex* vertices, unsigned int vertexCount, unsigned int* indices);

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	unsigned int m_indexCount;
};