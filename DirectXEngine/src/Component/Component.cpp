#include "Component.h"

#include "ComponentRegistry.h"

Component::Component(Entity& entity) : entity(entity), enabled(true)
{
	typeName = "";
}

Component::~Component()
{
}

void Component::debugAddInt(std::string label, int* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_INT, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddUInt(std::string label, unsigned int* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_UINT, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddBool(std::string label, bool* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_BOOL, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddFloat(std::string label, float* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_FLOAT, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddChar(std::string label, char* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_CHAR, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddString(std::string label, std::string* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_STRING, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddVec2(std::string label, DirectX::XMFLOAT2* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_VEC2, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddVec3(std::string label, DirectX::XMFLOAT3* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_VEC3, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddVec4(std::string label, DirectX::XMFLOAT4* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_VEC4, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddColor(std::string label, DirectX::XMFLOAT4* data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_COLOR, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddEnum(std::string label, void* data, unsigned int hash, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_ENUM, data, getterFunc, setterFunc, hash };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddStruct(std::string label, void* data, unsigned int hash, unsigned int stride, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_STRUCT, data, getterFunc, setterFunc, hash, stride };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddTexture(std::string label, Texture** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_TEXTURE, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddMaterial(std::string label, Material** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_MATERIAL, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddModel(std::string label, Mesh** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_MODEL, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddVertexShader(std::string label, VertexShader** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_VERTEX_SHADER, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddPixelShader(std::string label, PixelShader** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_PIXEL_SHADER, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddSampler(std::string label, Sampler** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_SAMPLER, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::debugAddFont(std::string label, Font** data, DebugGetterFunc getterFunc, DebugSetterFunc setterFunc)
{
	DebugComponentData componentData = { label, D_FONT, data, getterFunc, setterFunc };
	d_debugComponentData.push_back(componentData);
}

void Component::init()
{
	typeName = ComponentRegistry::getTypeName(typeid(*this));
}

void Component::initDebugVariables()
{
	debugAddBool("Enabled", &enabled);
}

void Component::update(float deltaTime, float totalTime)
{
}

void Component::lateUpdate(float deltaTime, float totalTime)
{
}

void Component::loadFromJSON(rapidjson::Value& dataObject)
{
	rapidjson::Value::MemberIterator componentEnabled = dataObject.FindMember("enabled");
	if (componentEnabled != dataObject.MemberEnd())
	{
		enabled = componentEnabled->value.GetBool();
	}
}

void Component::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("enabled");
	writer.Bool(enabled);
}

void Component::onSceneLoaded()
{
}

Entity& Component::getEntity() const
{
	return entity;
}

std::string Component::getName() const
{
	return typeName;
}

std::vector<DebugComponentData>& Component::getDebugComponentData()
{
	return d_debugComponentData;
}
