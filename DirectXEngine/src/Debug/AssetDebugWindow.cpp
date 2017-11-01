#include "AssetDebugWindow.h"

#include "../Scene/SceneManager.h"
#include "../Asset/AssetManager.h"

AssetDebugWindow::AssetDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 465' size='250 250' ").c_str());
}

AssetDebugWindow::~AssetDebugWindow()
{
}

void AssetDebugWindow::setupControls()
{
#if defined(DEBUG) || defined(_DEBUG)
	Scene* activeScene = SceneManager::getActiveScene();
	if (activeScene)
	{
		addSeparator("TextureSeparator", "Textures");
		TwAddVarRW(m_window, "TextureNameField", TW_TYPE_STDSTRING, &activeScene->d_textureNameField, " label='Texture Name' group='Textures' ");
		TwAddVarRW(m_window, "TexturePathField", TW_TYPE_STDSTRING, &activeScene->d_texturePathField, " label='Texture Path' group='Textures' ");
		addButton("AddTextureButton", "Load Texture", "Textures", &addTextureDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/Textures opened=false ").c_str());

		addSeparator("MaterialSeparator", "Materials");
		TwAddVarRW(m_window, "MaterialNameField", TW_TYPE_STDSTRING, &activeScene->d_materialNameField, " label='Material Name' group='Materials' ");
		TwAddVarRW(m_window, "MaterialPathField", TW_TYPE_STDSTRING, &activeScene->d_materialPathField, " label='Material Path' group='Materials' ");
		addButton("AddMaterialButton", "Load Material", "Materials", &addMaterialDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/Materials opened=false ").c_str());

		addSeparator("MeshSeparator", "Models");
		TwAddVarRW(m_window, "MeshNameField", TW_TYPE_STDSTRING, &activeScene->d_meshNameField, " label='Model Name' group='Models' ");
		TwAddVarRW(m_window, "MeshPathField", TW_TYPE_STDSTRING, &activeScene->d_meshPathField, " label='Model Path' group='Models' ");
		addButton("AddMeshButton", "Load Model", "Models", &addMeshDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/Models opened=false ").c_str());

		addSeparator("FontSeparator", "Fonts");
		TwAddVarRW(m_window, "FontNameField", TW_TYPE_STDSTRING, &activeScene->d_fontNameField, " label='Font Name' group='Fonts' ");
		TwAddVarRW(m_window, "FontPathField", TW_TYPE_STDSTRING, &activeScene->d_fontPathField, " label='Font Path' group='Fonts' ");
		addButton("AddFontButton", "Load Font", "Fonts", &addFontDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/Fonts opened=false ").c_str());

		addSeparator("SamplerSeparator", "Samplers");
		TwAddVarRW(m_window, "SamplerNameField", TW_TYPE_STDSTRING, &activeScene->d_samplerNameField, " label='Sampler Name' group='Samplers' ");
		TwAddVarRW(m_window, "SamplerPathField", TW_TYPE_STDSTRING, &activeScene->d_samplerPathField, " label='Sampler Path' group='Samplers' ");
		addButton("AddSamplerButton", "Load Sampler", "Samplers", &addSamplerDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/Samplers opened=false ").c_str());

		addSeparator("VertexShaderSeparator", "VertexShaders");
		TwAddVarRW(m_window, "VertexShaderNameField", TW_TYPE_STDSTRING, &activeScene->d_vertexShaderNameField, " label='Vertex Shader Name' group='VertexShaders' ");
		TwAddVarRW(m_window, "VertexShaderPathField", TW_TYPE_STDSTRING, &activeScene->d_vertexShaderPathField, " label='Vertex Shader Path' group='VertexShaders' ");
		addButton("AddVertexShaderButton", "Load Vertex Shader", "VertexShaders", &addVertexShaderDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/VertexShaders opened=false ").c_str());

		addSeparator("PixelShaderSeparator", "PixelShaders");
		TwAddVarRW(m_window, "PixelShaderNameField", TW_TYPE_STDSTRING, &activeScene->d_pixelShaderNameField, " label='Pixel Shader Name' group='PixelShaders' ");
		TwAddVarRW(m_window, "PixelShaderPathField", TW_TYPE_STDSTRING, &activeScene->d_pixelShaderPathField, " label='Pixel Shader Path' group='PixelShaders' ");
		addButton("AddPixelShaderButton", "Load Pixel Shader", "PixelShaders", &addPixelShaderDebugEditor, activeScene);
		TwDefine((" " + m_windowID + "/PixelShaders opened=false ").c_str());
	}
#endif
}

void AssetDebugWindow::addAsset(Texture* texture)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = texture->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default textures in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_TEXTURE_DIFFUSE || spacelessAssetID == DEFAULT_TEXTURE_SPECULAR
		|| spacelessAssetID == DEFAULT_TEXTURE_NORMAL || spacelessAssetID == DEFAULT_TEXTURE_GUI) return;

	std::string groupID = spacelessAssetID + "Texture";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Texture", groupID, &removeTextureDebugEditor, texture);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='Textures' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(Material* material)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = material->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default material in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_MATERIAL) return;

	std::string groupID = spacelessAssetID + "Material";

	std::string removeButtonID = groupID + "RemoveButton";
	
	addButton(removeButtonID, "Remove Material", groupID, &removeMaterialDebugEditor, material);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='Materials' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(Mesh* mesh)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = mesh->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	std::string groupID = spacelessAssetID + "Model";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Model", groupID, &removeMeshDebugEditor, mesh);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='Models' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(Font* font)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = font->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default font in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_FONT) return;

	std::string groupID = spacelessAssetID + "Font";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Font", groupID, &removeFontDebugEditor, font);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='Fonts' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(Sampler* sampler)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = sampler->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default sampler in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_SAMPLER) return;

	std::string groupID = spacelessAssetID + "Sampler";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Sampler", groupID, &removeSamplerDebugEditor, sampler);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='Samplers' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(VertexShader* vertexShader)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = vertexShader->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default vertex shader in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_SHADER_VERTEX) return;

	std::string groupID = spacelessAssetID + "VertexShader";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Vertex Shader", groupID, &removeVertexShaderDebugEditor, vertexShader);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='VertexShaders' opened=false ").c_str());
#endif
}

void AssetDebugWindow::addAsset(PixelShader* pixelShader)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string assetID = pixelShader->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	// Don't show the default pixel shader in the debug window, since it shouldn't be changed.
	if (spacelessAssetID == DEFAULT_SHADER_PIXEL) return;

	std::string groupID = spacelessAssetID + "PixelShaders";

	std::string removeButtonID = groupID + "RemoveButton";

	addButton(removeButtonID, "Remove Pixel Shader", groupID, &removePixelShaderDebugEditor, pixelShader);
	TwDefine((" " + m_windowID + "/" + groupID + " label='" + assetID + "' group='PixelShaders' opened=false ").c_str());
#endif
}

void AssetDebugWindow::removeAsset(Texture* texture)
{
	std::string assetID = texture->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "Texture").c_str());
}

void AssetDebugWindow::removeAsset(Material* material)
{
	std::string assetID = material->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "Material").c_str());
}

void AssetDebugWindow::removeAsset(Mesh* mesh)
{
	std::string assetID = mesh->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "Model").c_str());
}

void AssetDebugWindow::removeAsset(Font* font)
{
	std::string assetID = font->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "Font").c_str());
}

void AssetDebugWindow::removeAsset(Sampler* sampler)
{
	std::string assetID = sampler->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "Sampler").c_str());
}

void AssetDebugWindow::removeAsset(VertexShader* vertexShader)
{
	std::string assetID = vertexShader->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "VertexShader").c_str());
}

void AssetDebugWindow::removeAsset(PixelShader* pixelShader)
{
	std::string assetID = pixelShader->getAssetID();
	std::string spacelessAssetID = removeSpacesFromString(assetID);

	TwRemoveVar(m_window, (spacelessAssetID + "PixelShader").c_str());
}
