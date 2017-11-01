#pragma once
#include "DebugWindow.h"

class Material;
class Texture;
class Mesh;
class Font;
class Sampler;
class VertexShader;
class PixelShader;

class AssetDebugWindow : public DebugWindow
{
public:
	AssetDebugWindow(std::string windowID, std::string windowLabel);
	~AssetDebugWindow();

	void setupControls() override;

	void addAsset(Texture* texture);
	void addAsset(Material* material);
	void addAsset(Mesh* mesh);
	void addAsset(Font* font);
	void addAsset(Sampler* sampler);
	void addAsset(VertexShader* vertexShader);
	void addAsset(PixelShader* pixelShader);

	void removeAsset(Texture* texture);
	void removeAsset(Material* material);
	void removeAsset(Mesh* mesh);
	void removeAsset(Font* font);
	void removeAsset(Sampler* sampler);
	void removeAsset(VertexShader* vertexShader);
	void removeAsset(PixelShader* pixelShader);
};

void TW_CALL addTextureDebugEditor(void* clientData);
void TW_CALL addMaterialDebugEditor(void* clientData);
void TW_CALL addMeshDebugEditor(void* clientData);
void TW_CALL addFontDebugEditor(void* clientData);
void TW_CALL addSamplerDebugEditor(void* clientData);
void TW_CALL addVertexShaderDebugEditor(void* clientData);
void TW_CALL addPixelShaderDebugEditor(void* clientData);

void TW_CALL removeTextureDebugEditor(void* clientData);
void TW_CALL removeMaterialDebugEditor(void* clientData);
void TW_CALL removeMeshDebugEditor(void* clientData);
void TW_CALL removeFontDebugEditor(void* clientData);
void TW_CALL removeSamplerDebugEditor(void* clientData);
void TW_CALL removeVertexShaderDebugEditor(void* clientData);
void TW_CALL removePixelShaderDebugEditor(void* clientData);
