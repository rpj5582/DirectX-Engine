#pragma once
#include "DebugWindow.h"

class AssetDebugWindow : public DebugWindow
{
public:
	AssetDebugWindow(std::string windowID, std::string windowLabel);
	~AssetDebugWindow();

	void addTexture(std::string textureID);
	void addMaterial(std::string textureID);
	void addMesh(std::string textureID);
	void addSampler(std::string textureID);
	void addVertexShader(std::string textureID);
	void addPixelShader(std::string textureID);

private:

};