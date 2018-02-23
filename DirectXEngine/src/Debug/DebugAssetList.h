#pragma once
#include "IDebugEditor.h"

#include <Windows.h>
#include <string>

class DebugAssetList : public IDebugEditor
{
public:
	enum AssetType
	{
		UNDEF,
		TEXTURE,
		MATERIAL,
		MODEL,
		VERTEX_SHADER,
		PIXEL_SHADER,
		SAMPLER,
		FONT
	};

	DebugAssetList(HWND hWnd);
	~DebugAssetList();

	bool chooseTextureFile();
	bool chooseMaterialFile();
	bool chooseModelFile();
	bool chooseVertexShaderFile();
	bool choosePixelShaderFile();
	bool chooseSamplerFile();
	bool chooseFontFile();

	void openImportAssetNamePopup();
	void draw() override;

private:
	void handleImportAsset();

	HWND m_hWnd;

	AssetType m_type;
	std::string m_filepath;
};