#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Entity& entity) : RenderComponent(entity)
{
	castShadows = true;
	receiveShadows = true;

	m_mesh = nullptr;
	m_meshID = "";
}

MeshRenderComponent::~MeshRenderComponent()
{
}

void MeshRenderComponent::initDebugVariables()
{
	RenderComponent::initDebugVariables();

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Mesh", this, &getMeshRenderComponentMeshDebugEditor, &setMeshRenderComponentMeshDebugEditor, this);
	Debug::entityDebugWindow->addVariable(&castShadows, TW_TYPE_BOOLCPP, "Cast Shadows", this);
	Debug::entityDebugWindow->addVariable(&receiveShadows, TW_TYPE_BOOLCPP, "Receive Shadows", this);
}

void MeshRenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	RenderComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator mesh = dataObject.FindMember("mesh");
	if (mesh != dataObject.MemberEnd())
	{
		setMesh(mesh->value.GetString());
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

void MeshRenderComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	RenderComponent::saveToJSON(writer);

	writer.Key("mesh");
	writer.String(m_meshID.c_str());

	writer.Key("castShadows");
	writer.Bool(castShadows);

	writer.Key("receiveShadows");
	writer.Bool(receiveShadows);
}

Mesh* MeshRenderComponent::getMesh() const
{
	return m_mesh;
}

std::string MeshRenderComponent::getMeshID() const
{
	return m_meshID;
}

void MeshRenderComponent::setMesh(std::string meshID)
{
	m_mesh = AssetManager::getAsset<Mesh>(meshID);
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

void TW_CALL getMeshRenderComponentMeshDebugEditor(void* value, void* clientData)
{
	MeshRenderComponent* component = static_cast<MeshRenderComponent*>(clientData);
	std::string* meshInputField = static_cast<std::string*>(value);
	TwCopyStdStringToLibrary(*meshInputField, component->getMeshID());
}

void TW_CALL setMeshRenderComponentMeshDebugEditor(const void* value, void* clientData)
{
	MeshRenderComponent* component = static_cast<MeshRenderComponent*>(clientData);
	component->setMesh(*static_cast<const std::string*>(value));
}
