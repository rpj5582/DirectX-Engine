#include "DebugComponentList.h"

#include "../Scene/SceneManager.h"
#include "../Component/ComponentRegistry.h"

using namespace DirectX;

DebugComponentList::DebugComponentList()
{
	m_selectedEntityID = 0;
}

DebugComponentList::~DebugComponentList()
{
}

void DebugComponentList::setSelectedEntityID(unsigned int entityID)
{
	m_selectedEntityID = entityID;
}

void DebugComponentList::draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Component List", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

	if (m_selectedEntityID > 0)
	{
		std::vector<Entity*> entities = SceneManager::getActiveScene()->getAllEntities();
		for (unsigned int i = 0; i < entities.size(); i++)
		{
			if (m_selectedEntityID == entities[i]->getID())
			{
				std::vector<Component*> components = entities[i]->getAllComponents();
				for (unsigned int j = 0; j < components.size(); j++)
				{
					std::string componentName = components[j]->getName();

					bool shouldKeep = true; // Used for removing the component

					std::vector<DebugComponentData>& componentData = components[j]->getDebugComponentData();
					if (ImGui::CollapsingHeader((componentName + "##" + std::to_string(entities[i]->getID())).c_str(), &shouldKeep))
					{				
						for (unsigned int k = 0; k < componentData.size(); k++)
						{
							handleVariable(components[j], componentData[k]);
						}

						ImGui::Dummy(ImVec2(0.0f, 5.0f));
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0.0f, 5.0f));
					}

					if (!shouldKeep)
					{
						// Remove component
						entities[i]->removeComponent(components[j]);
					}
				}

				ImGui::Dummy(ImVec2(0.0f, 10.0f));

				ImGui::Indent(75.0f);
				if (ImGui::Button("Add Component", ImVec2(150, 40)))
				{
					ImGui::OpenPopup("Component Type");
				}

				if(ImGui::BeginPopup("Component Type"))
				{
					std::vector<std::string> typeNames = ComponentRegistry::getAllTypeNames();
					for (unsigned int k = 0; k < typeNames.size(); k++)
					{
						if (ImGui::MenuItem(typeNames[k].c_str()))
						{
							ComponentRegistry::addComponentToEntity(*entities[i], typeNames[k], true);
						}
					}

					ImGui::EndMenu();
				}
			}
		}
	}
	
	ImGui::End();

	ImGui::PopStyleVar(2);
}

bool DebugComponentList::handleVariable(Component* component, DebugComponentData& componentData)
{
	bool varModified = false;

	ImGui::PushItemWidth(150.0f);

	// Check the data type of the variable
	// Each data type should be handled similarly: if there are getter or setter functions,
	// use them, but if not just access the data directly.
	switch (componentData.type)
	{
	case D_BOOL:
	{
		bool b;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &b);
		else
			b = *static_cast<bool*>(componentData.data);

		if (ImGui::Checkbox(componentData.label.c_str(), &b))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &b);
			else
				*static_cast<bool*>(componentData.data) = b;
		}

		break;
	}

	case D_INT:
	{
		int i;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &i);
		else
			i = *static_cast<int*>(componentData.data);

		if (ImGui::DragInt(componentData.label.c_str(), &i))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &i);
			else
				*static_cast<int*>(componentData.data) = i;
		}

		break;
	}

	case D_UINT:
	{
		unsigned int i;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &i);
		else
			i = *static_cast<unsigned int*>(componentData.data);

		if (ImGui::DragInt(componentData.label.c_str(), (int*)&i, 1.0f, 0, UINTPTR_MAX))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &i);
			else
				*static_cast<unsigned int*>(componentData.data) = i;
		}

		break;
	}

	case D_FLOAT:
	{
		float f;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &f);
		else
			f = *static_cast<float*>(componentData.data);

		if (ImGui::DragFloat(componentData.label.c_str(), &f))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &f);
			else
				*static_cast<float*>(componentData.data) = f;
		}

		break;
	}

	case D_CHAR:
	{
		char c;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &c);
		else
			c = *static_cast<char*>(componentData.data);

		if (ImGui::InputText(componentData.label.c_str(), &c, 1))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &c);
			else
				*static_cast<char*>(componentData.data) = c;
		}

		break;
	}

	case D_STRING:
	{
		std::string s;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &s);
		else
			s = *static_cast<std::string*>(componentData.data);

		char c_str[64] = "";
		s._Copy_s(c_str, 64, s.size());

		if (ImGui::InputText(componentData.label.c_str(), c_str, IM_ARRAYSIZE(c_str), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			varModified = true;

			s = std::string(c_str);

			if (componentData.setterFunc)
				componentData.setterFunc(component, &s);
			else
				*static_cast<std::string*>(componentData.data) = s;
		}

		break;
	}

	case D_ENUM:
	{
		int e;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &e);
		else
			e = *static_cast<int*>(componentData.data);


		const char* displayString = Debug::getEnumDisplayString(componentData.hash);

		ImGui::PushItemWidth(150.0f);
		if (ImGui::Combo(componentData.label.c_str(), static_cast<int*>(&e), displayString))
		{
			varModified = true;

			if (componentData.setterFunc)
				componentData.setterFunc(component, &e);
			else
				*static_cast<int*>(componentData.data) = e;
		}

		ImGui::PopItemWidth();

		break;
	}

	case D_STRUCT:
	{
		std::vector<DebugComponentData>& structMembers = Debug::getStructMembers(componentData.hash);
		if (structMembers.size() > 0)
		{
			// Use recursion and pointer arithmetic to handle each member of the struct separately
			if (ImGui::CollapsingHeader(componentData.label.c_str()))
			{
				ImGui::Indent();

				unsigned int structSize = 0;
				for (unsigned int i = 0; i < structMembers.size(); i++)
				{
					structSize += structMembers[i].stride;
				}

				void* s = malloc(structSize);

				if (componentData.getterFunc)
					componentData.getterFunc(component, s);
				else
					memcpy(s, componentData.data, structSize);

				char* start = static_cast<char*>(s);
				for (unsigned int i = 0; i < structMembers.size(); i++)
				{
					DebugComponentData& member = structMembers[i];
					member.data = start;

					if (handleVariable(component, member))
					{
						varModified = true;

						if (componentData.setterFunc)
							componentData.setterFunc(component, s);
						else
							memcpy(componentData.data, s, structSize);
					}

					start += member.stride;
				}

				free(s);

				ImGui::Unindent();

				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
			}
		}

		break;
	}

	case D_VEC2:
	{
		XMFLOAT2 xmFloat2;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &xmFloat2);
		else
			xmFloat2 = *static_cast<XMFLOAT2*>(componentData.data);

		float float2[2] = { xmFloat2.x, xmFloat2.y };
		if (ImGui::DragFloat2(componentData.label.c_str(), float2))
		{
			varModified = true;

			xmFloat2.x = float2[0];
			xmFloat2.y = float2[1];

			if (componentData.setterFunc)
				componentData.setterFunc(component, &xmFloat2);
			else
				*static_cast<XMFLOAT2*>(componentData.data) = xmFloat2;
		}

		break;
	}

	case D_VEC3:
	{
		XMFLOAT3 xmFloat3;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &xmFloat3);
		else
			xmFloat3 = *static_cast<XMFLOAT3*>(componentData.data);

		float float3[3] = { xmFloat3.x, xmFloat3.y, xmFloat3.z };
		if (ImGui::DragFloat3(componentData.label.c_str(), float3))
		{
			varModified = true;

			xmFloat3.x = float3[0];
			xmFloat3.y = float3[1];
			xmFloat3.z = float3[2];

			if (componentData.setterFunc)
				componentData.setterFunc(component, &xmFloat3);
			else
				*static_cast<XMFLOAT3*>(componentData.data) = xmFloat3;
		}

		break;
	}

	case D_VEC4:
	{
		XMFLOAT4 xmFloat4;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &xmFloat4);
		else
			xmFloat4 = *static_cast<XMFLOAT4*>(componentData.data);

		float float4[4] = { xmFloat4.x, xmFloat4.y, xmFloat4.z, xmFloat4.w };
		if (ImGui::DragFloat4(componentData.label.c_str(), float4))
		{
			varModified = true;

			xmFloat4.x = float4[0];
			xmFloat4.y = float4[1];
			xmFloat4.z = float4[2];
			xmFloat4.w = float4[3];

			if (componentData.setterFunc)
				componentData.setterFunc(component, &xmFloat4);
			else
				*static_cast<XMFLOAT4*>(componentData.data) = xmFloat4;
		}

		break;
	}

	case D_COLOR:
	{
		XMFLOAT4 color;
		if (componentData.getterFunc)
			componentData.getterFunc(component, &color);
		else
			color = *static_cast<XMFLOAT4*>(componentData.data);

		float float4[4] = { color.x, color.y, color.z, color.w };
		if (ImGui::ColorEdit4(componentData.label.c_str(), float4, ImGuiColorEditFlags_NoInputs))
		{
			varModified = true;

			color.x = float4[0];
			color.y = float4[1];
			color.z = float4[2];
			color.w = float4[3];

			if (componentData.setterFunc)
				componentData.setterFunc(component, &color);
			else
				*static_cast<XMFLOAT4*>(componentData.data) = color;
		}

		break;
	}

	case D_TEXTURE:
	{
		handleAssetVariable<Texture>(component, componentData, "_TEXTURE");
		break;
	}

	case D_MATERIAL:
	{
		handleAssetVariable<Material>(component, componentData, "_MATERIAL");
		break;
	}

	case D_MODEL:
	{
		handleAssetVariable<Mesh>(component, componentData, "_MODEL");
		break;
	}

	case D_VERTEX_SHADER:
	{
		handleAssetVariable<VertexShader>(component, componentData, "_VERTEXSHADER");
		break;
	}

	case D_PIXEL_SHADER:
	{
		handleAssetVariable<PixelShader>(component, componentData, "_PIXELSHADER");
		break;
	}

	case D_SAMPLER:
	{
		handleAssetVariable<Sampler>(component, componentData, "_SAMPLER");
		break;
	}

	case D_FONT:
	{
		handleAssetVariable<Font>(component, componentData, "_FONT");
		break;
	}

	default:
		break;
	}

	ImGui::PopItemWidth();

	return varModified;
}
