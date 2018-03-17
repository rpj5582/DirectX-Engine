#include "DebugMainMenuBar.h"

#include "../Third Party/imgui/imgui.h"
#include "../Game.h"

DebugMainMenuBar::DebugMainMenuBar(DebugEntityList& entityList, DebugComponentList& componentList, DebugAssetList& assetList)
	: m_entityList(entityList), m_componentList(componentList), m_assetList(assetList)
{
	m_drawEntityList = true;
	m_drawComponentList = true;
	m_drawAssetList = true;
	m_drawConsole = true;

	m_closedState = NOT_CLOSED;
}

DebugMainMenuBar::~DebugMainMenuBar()
{
}

float DebugMainMenuBar::getHeight() const
{
	return m_height;
}

bool DebugMainMenuBar::shouldDrawEntityList() const
{
	return m_drawEntityList;
}

bool DebugMainMenuBar::shouldDrawComponentList() const
{
	return m_drawComponentList;
}

bool DebugMainMenuBar::shouldDrawAssetList() const
{
	return m_drawAssetList;
}

bool DebugMainMenuBar::shouldDrawConsole() const
{
	return m_drawConsole;
}

bool DebugMainMenuBar::chooseNew()
{
	if (SceneManager::getActiveScene()->isDirty())
		return false;
	else
	{
		SceneManager::newScene();
		return true;
	}
}

bool DebugMainMenuBar::chooseLoad()
{
	if (SceneManager::getActiveScene()->isDirty())
		return false;
	else
	{
		SceneManager::loadScene();
		return true;
	}
}

bool DebugMainMenuBar::chooseQuit()
{
	if (SceneManager::getActiveScene()->isDirty())
	{
		m_closedState = TRYING_TO_CLOSE;
		return false;
	}
	else
	{
		m_closedState = CLOSED;
		Game::Quit();
		return true;
	}	
}

void DebugMainMenuBar::draw()
{
	bool showNewSaveChangesModal = false;
	bool showLoadSaveChangesModal = false;
	bool showQuitSaveChangesModal = false;

	bool openAssetNamePopup = false;

	if (ImGui::BeginMainMenuBar())
	{
		m_height = ImGui::GetWindowHeight();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "CTRL-N"))
			{
				showNewSaveChangesModal = !chooseNew();
			}

			if (ImGui::MenuItem("Open Scene", "CTRL-O"))
			{
				showLoadSaveChangesModal = !chooseLoad();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save Scene", "CTRL-S"))
			{
				SceneManager::saveActiveScene();
			}

			if (ImGui::MenuItem("Save Scene As", "CTRL-SHIFT-S"))
			{
				SceneManager::saveActiveSceneAs();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "ALT-F4"))
			{
				showQuitSaveChangesModal = !chooseQuit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Create Blank"))
			{
				Entity* entity = SceneManager::getActiveScene()->createEntity("entity");
				entity->addComponent<Transform>();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Create Mesh"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("cube");
					entity->addComponent<Transform>();
					MeshRenderComponent* meshRenderer = entity->addComponent<MeshRenderComponent>();
					meshRenderer->setMesh(AssetManager::getAsset<Mesh>(DEFAULT_MODEL_CUBE));
				}

				if (ImGui::MenuItem("Sphere"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("sphere");
					entity->addComponent<Transform>();
					MeshRenderComponent* meshRenderer = entity->addComponent<MeshRenderComponent>();
					meshRenderer->setMesh(AssetManager::getAsset<Mesh>(DEFAULT_MODEL_SPHERE));
				}

				if (ImGui::MenuItem("Capsule"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("capsule");
					entity->addComponent<Transform>();
					MeshRenderComponent* meshRenderer = entity->addComponent<MeshRenderComponent>();
					meshRenderer->setMesh(AssetManager::getAsset<Mesh>(DEFAULT_MODEL_CAPSULE));
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Create Camera"))
			{
				Entity* entity = SceneManager::getActiveScene()->createEntity("camera");
				entity->addComponent<Transform>();
				entity->addComponent<CameraComponent>();
			}

			if (ImGui::MenuItem("Create Light"))
			{
				Entity* entity = SceneManager::getActiveScene()->createEntity("light");
				entity->addComponent<Transform>();
				entity->addComponent<LightComponent>();
			}

			if (ImGui::BeginMenu("Create GUI"))
			{
				if (ImGui::MenuItem("Blank"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("gui");
					entity->addComponent<GUITransform>();
				}

				if (ImGui::MenuItem("Text"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("text");
					entity->addComponent<GUITransform>();
					entity->addComponent<GUITextComponent>();
				}

				if (ImGui::MenuItem("Sprite"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("sprite");
					entity->addComponent<GUITransform>();
					entity->addComponent<GUISpriteComponent>();
				}

				if (ImGui::MenuItem("Button"))
				{
					Entity* entity = SceneManager::getActiveScene()->createEntity("button");
					entity->addComponent<GUITransform>();
					entity->addComponent<GUIButtonComponent>();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Asset"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Texture"))
				{
					
				}

				if (ImGui::MenuItem("Material"))
				{

				}

				if (ImGui::MenuItem("Model"))
				{

				}

				if (ImGui::MenuItem("Shader"))
				{

				}

				if (ImGui::MenuItem("Sampler"))
				{

				}

				if (ImGui::MenuItem("Font"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Import"))
			{
				if (ImGui::MenuItem("Texture"))
				{
					if (m_assetList.chooseTextureFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Material"))
				{
					if (m_assetList.chooseMaterialFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Model"))
				{
					if (m_assetList.chooseModelFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Vertex Shader"))
				{
					if (m_assetList.chooseVertexShaderFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Pixel Shader"))
				{
					if (m_assetList.choosePixelShaderFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Sampler"))
				{
					if (m_assetList.chooseSamplerFile())
						openAssetNamePopup = true;
				}

				if (ImGui::MenuItem("Font"))
				{
					if (m_assetList.chooseFontFile())
						openAssetNamePopup = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::Checkbox("Show Entity List", &m_drawEntityList);
			ImGui::Checkbox("Show Component List", &m_drawComponentList);
			ImGui::Checkbox("Show Asset List", &m_drawAssetList);
			ImGui::Checkbox("Show Console", &m_drawConsole);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// Save changes popup for the New Scene menu item
	if (showNewSaveChangesModal)
	{
		openNewSaveChangesPopup();
	}

	handleNewSaveChanges();

	// Save changes popup for the Load Scene menu item
	if (showLoadSaveChangesModal)
	{
		openLoadSaveChangesPopup();
	}

	handleLoadSaveChanges();

	// Save changes popup for the Quit menu item
	if (showQuitSaveChangesModal)
	{
		openQuitSaveChangesPopup();
	}

	handleQuitSaveChanges();

	// Open asset name popup
	if (openAssetNamePopup)
		m_assetList.openImportAssetNamePopup();
}

void DebugMainMenuBar::openNewSaveChangesPopup()
{
	ImGui::OpenPopup("Save Changes##New");
}

void DebugMainMenuBar::openLoadSaveChangesPopup()
{
	ImGui::OpenPopup("Save Changes##Load");
}

void DebugMainMenuBar::openQuitSaveChangesPopup()
{
	ImGui::OpenPopup("Save Changes##Quit");
}

ClosedState DebugMainMenuBar::getAppClosedState() const
{
	return m_closedState;
}

void DebugMainMenuBar::setAppClosedState(ClosedState state)
{
	m_closedState = state;
}

void DebugMainMenuBar::handleNewSaveChanges()
{
	if (ImGui::BeginPopupModal("Save Changes##New", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Do you want to save your changes before closing the scene?");
		if (ImGui::Button("Yes", ImVec2(120, 0)))
		{
			if (SceneManager::saveActiveScene())
				SceneManager::newScene();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0)))
		{
			SceneManager::newScene();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void DebugMainMenuBar::handleLoadSaveChanges()
{
	if (ImGui::BeginPopupModal("Save Changes##Load", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Do you want to save your changes before closing the scene?");
		if (ImGui::Button("Yes", ImVec2(120, 0)))
		{
			if (SceneManager::saveActiveScene())
				SceneManager::loadScene();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0)))
		{
			SceneManager::loadScene();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void DebugMainMenuBar::handleQuitSaveChanges()
{
	if (ImGui::BeginPopupModal("Save Changes##Quit", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Do you want to save your changes before quitting?");
		if (ImGui::Button("Yes", ImVec2(120, 0)))
		{
			if (SceneManager::saveActiveScene())
			{
				Game::Quit();
				m_closedState = CLOSED;
			}
			else
				m_closedState = NOT_CLOSED;

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0)))
		{
			Game::Quit();
			ImGui::CloseCurrentPopup();
			m_closedState = CLOSED;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_closedState = NOT_CLOSED;
		}

		ImGui::EndPopup();
	}
}
