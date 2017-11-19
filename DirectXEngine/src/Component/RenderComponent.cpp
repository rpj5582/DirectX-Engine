#include "RenderComponent.h"

using namespace DirectX;

TwEnumVal RenderComponent::d_renderStyleMembers[3] = 
{
	{ SOLID, "Solid" }, 
	{ WIREFRAME, "Wireframe" }, 
	{ SOLID_WIREFRAME, "Solid Wireframe" }
};

TwType RenderComponent::TW_TYPE_RENDER_STLYE = TW_TYPE_UNDEF;

RenderComponent::RenderComponent(Entity& entity) : Component(entity)
{
	m_material = nullptr;
	m_renderStyle = RenderStyle::SOLID;
	m_wireColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::init()
{
	Component::init();

	setMaterial(DEFAULT_MATERIAL);
}

void RenderComponent::initDebugVariables()
{
	Component::initDebugVariables();

	if (TW_TYPE_RENDER_STLYE == TW_TYPE_UNDEF)
		TW_TYPE_RENDER_STLYE = TwDefineEnum("TW_TYPE_RENDER_STLYE", d_renderStyleMembers, 3);

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Material", this, &getRenderComponentMaterialDebugEditor, &setRenderComponentMaterialDebugEditor, this);
	Debug::entityDebugWindow->addVariable(&m_renderStyle, TW_TYPE_RENDER_STLYE, "Render Style", this);
	Debug::entityDebugWindow->addVariable(&m_wireColor, TW_TYPE_COLOR4F, "Wireframe Color", this);
}

void RenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator material = dataObject.FindMember("material");
	if (material != dataObject.MemberEnd())
	{
		setMaterial(material->value.GetString());
	}

	rapidjson::Value::MemberIterator renderStyle = dataObject.FindMember("renderStyle");
	if (renderStyle != dataObject.MemberEnd())
	{
		const char* renderStyleString = renderStyle->value.GetString();
		switch (stringHash(renderStyleString))
		{
		case stringHash("solid"):
		{
			m_renderStyle = SOLID;
			break;
		}

		case stringHash("wireframe"):
		{
			m_renderStyle = WIREFRAME;
			break;
		}

		case stringHash("solid-wireframe"):
		{
			m_renderStyle = SOLID_WIREFRAME;
			break;
		}

		default:
			Debug::warning("Invalid render style " + std::string(renderStyleString) + " on RenderComponent of entity " + entity.getName() + ". Keeping default of SOLID.");
			break;
		}
	}

	rapidjson::Value::MemberIterator wireColor = dataObject.FindMember("wireColor");
	if (wireColor != dataObject.MemberEnd())
	{
		m_wireColor = XMFLOAT4(wireColor->value["r"].GetFloat(), wireColor->value["g"].GetFloat(), wireColor->value["b"].GetFloat(), wireColor->value["a"].GetFloat());
	}
}

void RenderComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("material");
	writer.String(m_materialID.c_str());

	writer.Key("renderStyle");
	switch (m_renderStyle)
	{
	case SOLID:
		writer.String("solid");
		break;

	case WIREFRAME:
		writer.String("wireframe");
		break;

	case SOLID_WIREFRAME:
		writer.String("solid-wireframe");
		break;

	default:
		Debug::warning("Invalid render style " + std::to_string(m_renderStyle) + " on RenderComponent of entity " + entity.getName() + ". Saving default of SOLID.");
		writer.String("solid");
		break;
	}

	writer.Key("wireColor");
	writer.StartObject();

	writer.Key("r");
	writer.Double((double)m_wireColor.x);

	writer.Key("g");
	writer.Double((double)m_wireColor.y);

	writer.Key("b");
	writer.Double((double)m_wireColor.z);

	writer.Key("a");
	writer.Double((double)m_wireColor.w);

	writer.EndObject();
}

Material* RenderComponent::getMaterial() const
{
	return m_material;
}

std::string RenderComponent::getMaterialID() const
{
	return m_materialID;
}

void RenderComponent::setMaterial(std::string materialID)
{
	if (materialID == "")
	{
		m_material = AssetManager::getAsset<Material>(DEFAULT_MATERIAL);
		m_materialID = DEFAULT_MATERIAL;
		return;
	}

	Material* material = AssetManager::getAsset<Material>(materialID);
	if (material)
	{
		m_material = material;
		m_materialID = materialID;
	}	
}

RenderStyle RenderComponent::getRenderStyle() const
{
	return m_renderStyle;
}

void RenderComponent::setRenderStyle(RenderStyle renderStyle)
{
	m_renderStyle = renderStyle;
}

XMFLOAT4 RenderComponent::getWireframeColor() const
{
	return m_wireColor;
}

void RenderComponent::setWireframeColor(XMFLOAT4 color)
{
	m_wireColor = color;
}

void TW_CALL getRenderComponentMaterialDebugEditor(void* value, void* clientData)
{
	RenderComponent* component = static_cast<RenderComponent*>(clientData);
	std::string* materialInputField = static_cast<std::string*>(value);

	TwCopyStdStringToLibrary(*materialInputField, component->getMaterialID());
}

void TW_CALL setRenderComponentMaterialDebugEditor(const void* value, void* clientData)
{
	RenderComponent* component = static_cast<RenderComponent*>(clientData);
	component->setMaterial(*static_cast<const std::string*>(value));
}
