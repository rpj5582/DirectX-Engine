#pragma once

#include "Output.h"

#include "Mesh.h"
#include "Material.h"

#include <SpriteFont.h>
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

	bool init();

	static Mesh* getMesh(std::string id);
	static Material* getMaterial(std::string id);
	static ID3D11ShaderResourceView* getTexture(std::string id);
	static ID3D11SamplerState* getSampler(std::string id);
	static SimpleVertexShader* getVertexShader(std::string id);
	static SimplePixelShader* getPixelShader(std::string id);
	static DirectX::SpriteFont* getFont(std::string id);

	static Material* createMaterial(std::string id);
	static Material* createMaterial(std::string id, std::string diffuseTextureID);
	static Material* createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID);
	static Material* createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID);
	static Material* createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID, std::string samplerID);
	static Material* createMaterial(std::string id, std::string diffuseTextureID, std::string specularTextureID, std::string normalTextureID, std::string samplerID, std::string vertexShaderID, std::string pixelShaderID);

	static ID3D11SamplerState* createSampler(std::string id, const D3D11_SAMPLER_DESC& samplerDesc);

	static bool loadShader(std::string id, ShaderType type, std::string filepath);

	static Mesh* loadMesh(std::string id, std::string filepath);
	static Mesh* loadMesh(std::string id, Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount);

	static ID3D11ShaderResourceView* loadTexture(std::string id, std::string filepath);
	static ID3D11ShaderResourceView* createSolidColorTexture(std::string id, unsigned int hexColor);

	static DirectX::SpriteFont* loadFont(std::string id, std::string filepath);

private:
	// A helper function when loading meshes to calculate tangents for the normal map lighting calculation.
	void calculateTangents(Vertex* vertices, unsigned int vertexCount, unsigned int* indices);

	static AssetManager* m_instance;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::unordered_map<std::string, Mesh*> m_meshes;
	std::unordered_map<std::string, Material*> m_materials;

	std::unordered_map<std::string, SimpleVertexShader*> m_vertexShaders;
	std::unordered_map<std::string, SimplePixelShader*> m_pixelShaders;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_textures;
	std::unordered_map<std::string, ID3D11SamplerState*> m_samplers;

	std::unordered_map<std::string, DirectX::SpriteFont*> m_fonts;
};