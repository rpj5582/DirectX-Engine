#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Entity& entity) : RenderComponent(entity)
{
	m_mesh = nullptr;
	m_meshID = "";
}

MeshRenderComponent::~MeshRenderComponent()
{
}

void MeshRenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	RenderComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator mesh = dataObject.FindMember("mesh");
	if (mesh != dataObject.MemberEnd())
	{
		setMesh(mesh->value.GetString());
	}
}

void MeshRenderComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	RenderComponent::saveToJSON(writer);

	writer.Key("mesh");
	writer.String(m_meshID.c_str());
}

Mesh* MeshRenderComponent::getMesh() const
{
	return m_mesh;
}

void MeshRenderComponent::setMesh(std::string meshID)
{
	m_mesh = AssetManager::getMesh(meshID);
	if (m_mesh)
	{
		m_meshID = meshID;
	}
	else
	{
		m_meshID = "";
		m_mesh = nullptr;
	}
}
