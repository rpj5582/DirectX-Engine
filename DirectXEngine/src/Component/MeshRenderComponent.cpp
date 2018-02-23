#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Entity& entity) : RenderComponent(entity)
{
	castShadows = true;
	receiveShadows = true;

	m_mesh = nullptr;
}

MeshRenderComponent::~MeshRenderComponent()
{
}

void MeshRenderComponent::initDebugVariables()
{
	RenderComponent::initDebugVariables();
	
	Mesh** mesh = &m_mesh;
	debugAddModel("Model", &m_mesh);

	debugAddBool("Cast Shadows", &castShadows);
	debugAddBool("Receive Shadows", &receiveShadows);
}

void MeshRenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	RenderComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator mesh = dataObject.FindMember("mesh");
	if (mesh != dataObject.MemberEnd())
	{
		setMesh(AssetManager::getAsset<Mesh>(mesh->value.GetString()));
	}

	rapidjson::Value::MemberIterator castShadowsIter = dataObject.FindMember("castShadows");
	if (castShadowsIter != dataObject.MemberEnd())
	{
		castShadows = castShadowsIter->value.GetBool();
	}

	rapidjson::Value::MemberIterator receiveShadowsIter = dataObject.FindMember("receiveShadows");
	if (receiveShadowsIter != dataObject.MemberEnd())
	{
		receiveShadows = receiveShadowsIter->value.GetBool();
	}
}

void MeshRenderComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	RenderComponent::saveToJSON(writer);

	if (m_mesh)
	{
		writer.Key("mesh");
		writer.String(m_mesh->getAssetID().c_str());
	}

	writer.Key("castShadows");
	writer.Bool(castShadows);

	writer.Key("receiveShadows");
	writer.Bool(receiveShadows);
}

Mesh* MeshRenderComponent::getMesh() const
{
	return m_mesh;
}

void MeshRenderComponent::setMesh(Mesh* mesh)
{
	m_mesh = mesh;
}
