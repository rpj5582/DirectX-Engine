#pragma once

#include "DXCore.h"
#include "SimpleShader.h"

class Material
{
public:
	Material(ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader);
	~Material();

	SimpleVertexShader* getVertexShader() const;
	SimplePixelShader* getPixelShader() const;

	void useMaterial();

private:
	SimpleVertexShader* m_vertexShader;
	SimplePixelShader* m_pixelShader;
};