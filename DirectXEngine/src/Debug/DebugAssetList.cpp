#include "DebugAssetList.h"

#include "../Asset/AssetManager.h"
#include "../Util.h"

DebugAssetList::DebugAssetList(HWND hWnd)
{
	m_hWnd = hWnd;
}

DebugAssetList::~DebugAssetList()
{
}

bool DebugAssetList::chooseTextureFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "Image File\0*.png;*.jpg;*.bmp;*.tiff;*.gif");
	if (m_filepath != "")
	{
		m_type = TEXTURE;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("tex_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::chooseMaterialFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "JSON Material File\0*.json");
	if (m_filepath != "")
	{
		m_type = MATERIAL;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("mat_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::chooseModelFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "OBJ Model File\0*.obj");
	if (m_filepath != "")
	{
		m_type = MODEL;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("model_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::chooseVertexShaderFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "Compiled Shader Object File\0*.cso");
	if (m_filepath != "")
	{
		m_type = VERTEX_SHADER;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("vs_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::choosePixelShaderFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "Compiled Shader Object File\0*.cso");
	if (m_filepath != "")
	{
		m_type = PIXEL_SHADER;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("ps_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::chooseSamplerFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "JSON Sampler File\0*.json");
	if (m_filepath != "")
	{
		m_type = SAMPLER;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("samp_" + filename));
		return true;
	}

	return false;
}

bool DebugAssetList::chooseFontFile()
{
	m_filepath = Util::loadFileDialog(m_hWnd, "SpriteFont File\0*.spritefont");
	if (m_filepath != "")
	{
		m_type = FONT;

		std::string::size_type dotPos = m_filepath.find_last_of('.');
		std::string filename = m_filepath.substr(0, dotPos);

		std::string::size_type slashPos = filename.find_last_of("\\/");
		filename = filename.substr(slashPos + 1, filename.size() - slashPos);

		setPopupInputText(("font_" + filename));
		return true;
	}

	return false;
}

void DebugAssetList::handleImportAsset()
{
	std::string inputText;
	Choice choice = showTextInputPopup("Enter Asset Name", "Enter a unique name for this asset.", inputText);
	switch (choice)
	{
	case OK:
	{
		switch (m_type)
		{
		case TEXTURE:
		{
			if (AssetManager::loadAsset<Texture>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case MATERIAL:
		{
			if (AssetManager::loadAsset<Material>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case MODEL:
		{
			if (AssetManager::loadAsset<Mesh>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case VERTEX_SHADER:
		{
			if (AssetManager::loadAsset<VertexShader>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case PIXEL_SHADER:
		{
			if (AssetManager::loadAsset<PixelShader>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case SAMPLER:
		{
			if (AssetManager::loadAsset<Sampler>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		case FONT:
		{
			if (AssetManager::loadAsset<Font>(inputText, m_filepath))
				Debug::message("Loaded asset " + inputText + " successfully.");

			m_type = UNDEF;
			m_filepath = "";
			break;
		}

		default:
			break;
		}

		break;
	}

	case CANCEL:
	{
		m_type = UNDEF;
		m_filepath = "";
		break;
	}

	case INVALID:
	{
		Debug::warning("You must enter an asset name.");
		break;
	}

	default:
		break;
	}
}

void DebugAssetList::openImportAssetNamePopup()
{
	ImGui::OpenPopup("Enter Asset Name");
}

void DebugAssetList::draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	
	ImGui::BeginChild("AssetsChild", ImVec2(0.0f, 175.0f));

	if (ImGui::CollapsingHeader("Textures"))
	{
		std::vector<Texture*> textures = AssetManager::getAllAssets<Texture>();
		ImGui::Indent();
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			ImGui::Text(textures[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_TEXTURE", &textures[i], sizeof(Texture*));
				ImGui::Text(("TEXTURE: " + textures[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(textures[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<Texture>(textures[i]);
				}
			}

			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Materials"))
	{
		std::vector<Material*> materials = AssetManager::getAllAssets<Material>();
		ImGui::Indent();
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			ImGui::Text(materials[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_MATERIAL", &materials[i], sizeof(Material*));
				ImGui::Text(("MATERIAL: " + materials[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(materials[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<Material>(materials[i]);
				}
			}
			
			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Models"))
	{
		std::vector<Mesh*> models = AssetManager::getAllAssets<Mesh>();
		ImGui::Indent();
		for (unsigned int i = 0; i < models.size(); i++)
		{
			ImGui::Text(models[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_MODEL", &models[i], sizeof(Mesh*));
				ImGui::Text(("MODEL: " + models[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(models[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<Mesh>(models[i]);
				}
			}

			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Vertex Shaders"))
	{
		std::vector<VertexShader*> vertexShaders = AssetManager::getAllAssets<VertexShader>();
		ImGui::Indent();
		for (unsigned int i = 0; i < vertexShaders.size(); i++)
		{
			ImGui::Text(vertexShaders[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_VERTEXSHADER", &vertexShaders[i], sizeof(VertexShader*));
				ImGui::Text(("VERTEX SHADER: " + vertexShaders[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(vertexShaders[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<VertexShader>(vertexShaders[i]);
				}
			}

			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Pixel Shaders"))
	{
		std::vector<PixelShader*> pixelShaders = AssetManager::getAllAssets<PixelShader>();
		ImGui::Indent();
		for (unsigned int i = 0; i < pixelShaders.size(); i++)
		{
			ImGui::Text(pixelShaders[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_PIXELSHADER", &pixelShaders[i], sizeof(PixelShader*));
				ImGui::Text(("PIXEL SHADER: " + pixelShaders[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(pixelShaders[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<PixelShader>(pixelShaders[i]);
				}
			}

			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Samplers"))
	{
		std::vector<Sampler*> samplers = AssetManager::getAllAssets<Sampler>();
		ImGui::Indent();
		for (unsigned int i = 0; i < samplers.size(); i++)
		{
			ImGui::Text(samplers[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("_SAMPLER", &samplers[i], sizeof(Sampler*));
				ImGui::Text(("SAMPLER: " + samplers[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}
			
			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(samplers[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<Sampler>(samplers[i]);
				}
			}
			
			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Fonts"))
	{
		std::vector<Font*> fonts = AssetManager::getAllAssets<Font>();
		ImGui::Indent();
		for (unsigned int i = 0; i < fonts.size(); i++)
		{
			ImGui::Text(fonts[i]->getAssetID().c_str());
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("_FONT", &fonts[i], sizeof(Font*));
				ImGui::Text(("FONT: " + fonts[i]->getAssetID()).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::SameLine();
			ImGui::Indent(io.DisplaySize.x - 800.0f);

			if (ImGui::Button("Details", ImVec2(65.0f, 18.0f)))
			{

			}

			if (!AssetManager::isDefaultAsset(fonts[i]->getAssetID()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Remove", ImVec2(65.0f, 18.0f)))
				{
					AssetManager::unloadAsset<Font>(fonts[i]);
				}
			}	

			ImGui::Unindent(io.DisplaySize.x - 800.0f);
		}
		ImGui::Unindent();
	}

	ImGui::EndChild();

	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Indent((io.DisplaySize.x / 2.0f - 300.0f) / 2.0f - 160.0f);
	if (ImGui::Button("Create Asset", ImVec2(150.0f, 30.0f)))
	{
		
	}

	ImGui::SameLine();
	
	ImGui::Indent(160.0f);
	if (ImGui::Button("Import Asset", ImVec2(150.0f, 30.0f)))
	{
		ImGui::OpenPopup("ImportAssetPopupMenu");
	}

	bool showAssetNamePopup = false;

	if (ImGui::BeginPopup("ImportAssetPopupMenu"))
	{
		if (ImGui::MenuItem("Texture"))
		{
			showAssetNamePopup = chooseTextureFile();
		}

		if (ImGui::MenuItem("Material"))
		{
			showAssetNamePopup = chooseMaterialFile();
		}

		if (ImGui::MenuItem("Model"))
		{
			showAssetNamePopup = chooseModelFile();
		}

		if (ImGui::MenuItem("Vertex Shader"))
		{
			showAssetNamePopup = chooseVertexShaderFile();
		}

		if (ImGui::MenuItem("Pixel Shader"))
		{
			showAssetNamePopup = choosePixelShaderFile();
		}

		if (ImGui::MenuItem("Sampler"))
		{
			showAssetNamePopup = chooseSamplerFile();
		}

		if (ImGui::MenuItem("Font"))
		{
			showAssetNamePopup = chooseFontFile();
		}

		ImGui::EndPopup();
	}

	ImGui::End();

	ImGui::PopStyleVar();

	if (showAssetNamePopup)
		openImportAssetNamePopup();

	handleImportAsset();
}
