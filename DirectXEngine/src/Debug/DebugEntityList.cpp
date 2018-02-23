#include "DebugEntityList.h"

#include "../Scene/SceneManager.h"

unsigned int DebugEntityList::m_selectedEntityID = 0;

DebugEntityList::DebugEntityList()
{
}

DebugEntityList::~DebugEntityList()
{
}

unsigned int DebugEntityList::getSelectedEntity() const
{
	return m_selectedEntityID;
}

void DebugEntityList::draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Entity List", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

	ImGui::TreePush("Entity List");

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
	{
		m_selectedEntityID = 0;
	}

	std::vector<Entity*> entities = SceneManager::getActiveScene()->getAllEntities();

	// Filter the entities to only include root entities, so we can properly step through the children trees.
	std::vector<Entity*> rootEntities;
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->getParent() == nullptr)
			rootEntities.push_back(entities[i]);
	}

	for (unsigned int i = 0; i < rootEntities.size(); i++)
	{
		handleEntity(*rootEntities[i]);
	}

	ImGui::TreePop();

	ImGui::End();

	ImGui::PopStyleVar(2);
}

void DebugEntityList::handleEntity(Entity& entity)
{
	unsigned int id = entity.getID();

	bool opened = ImGui::TreeNodeEx((void*)(uintptr_t)id, ImGuiTreeNodeFlags_OpenOnArrow | ((id == m_selectedEntityID) ? ImGuiTreeNodeFlags_Selected : 0), entity.getName().c_str());

	if (ImGui::IsItemClicked())
	{
		m_selectedEntityID = id;
	}
	else if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup(("Entity Context Menu##" + std::to_string(entity.getID())).c_str());
	}

	if (ImGui::BeginPopup(("Entity Context Menu##" + std::to_string(entity.getID())).c_str()))
	{
		bool shouldRename = false;
		if (ImGui::MenuItem("Rename"))
		{
			shouldRename = true;
		}

		if (ImGui::BeginMenu("Tag"))
		{
			std::vector<std::string> tags = SceneManager::getActiveScene()->getAllTags();
			for (unsigned int i = 0; i < tags.size(); i++)
			{
				bool tagged = entity.hasTag(tags[i]);
				if (ImGui::Checkbox(tags[i].c_str(), &tagged))
				{
					if (tagged)
					{
						entity.addTag(tags[i]);
					}
					else
					{
						entity.removeTag(tags[i]);
					}
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Delete"))
		{
			SceneManager::getActiveScene()->deleteEntity(&entity);
			ImGui::EndPopup();
			return;
		}

		ImGui::EndPopup();

		if(shouldRename)
			ImGui::OpenPopup("Rename Entity");
	}

	if (opened)
	{
		std::vector<Entity*> children = entity.getChildren();
		for (unsigned int j = 0; j < children.size(); j++)
		{
			handleEntity(*children[j]);
		}

		ImGui::TreePop();
	}

	std::string name;
	Choice choice = showTextInputPopup("Rename Entity", "Enter a new entity name.", name);
	switch (choice)
	{
	case OK:
	{
		entity.rename(name);
		break;
	}
		
	case INVALID:
	{
		Debug::warning("You must enter a valid entity name.");
		break;
	}
		
	default:
		break;
	}
}
