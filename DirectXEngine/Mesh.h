#pragma once

#include "Vertex.h"
#include <d3d11.h>

class Mesh
{
public:
	Mesh(ID3D11Device* device, Vertex* vertices, unsigned int vertexCount, UINT* indices, unsigned int indexCount);
	~Mesh();

	ID3D11Buffer* getVertexBuffer() const;
	ID3D11Buffer* getIndexBuffer() const;

	unsigned int getIndexCount() const;

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	unsigned int m_indexCount;
};