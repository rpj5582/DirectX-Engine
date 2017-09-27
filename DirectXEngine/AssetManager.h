#pragma once

#include "Mesh.h"
#include "Material.h"

#include <unordered_map>

enum ShaderType
{
	VertexShader,
	PixelShader
};

class AssetManager
{
public:
	AssetManager(ID3D11Device* device, ID3D11DeviceContext* context);
	~AssetManager();

	Mesh* addMesh(const char* id, const char* filename);
	Mesh* addMesh(const char* id, Vertex* vertices, int vertexCount, UINT* indices, int indexCount);
	Material* addMaterial(const char* id, const char* vertexShaderID, const char* pixelShaderID);

	void loadShader(const char* id, ShaderType type, LPCWSTR filename);

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::unordered_map<const char*, Mesh*> m_meshes;
	std::unordered_map<const char*, Material*> m_materials;

	std::unordered_map<const char*, SimpleVertexShader*> m_vertexShaders;
	std::unordered_map<const char*, SimplePixelShader*> m_pixelShaders;
};