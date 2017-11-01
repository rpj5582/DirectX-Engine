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
