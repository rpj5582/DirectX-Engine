#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Entity& entity) : RenderComponent(entity)
{
	m_mesh = nullptr;
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
		m_mesh = AssetManager::getMesh(mesh->value.GetString());
	}
}

Mesh* MeshRenderComponent::getMesh() const
{
	return m_mesh;
}

void MeshRenderComponent::setMesh(Mesh* mesh)
{
	m_mesh = mesh;
}
