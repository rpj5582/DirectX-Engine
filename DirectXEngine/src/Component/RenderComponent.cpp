#include "RenderComponent.h"

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
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::init()
{
	Component::init();

	setMaterial(DEFAULT_MATERIAL);

	if(TW_TYPE_RENDER_STLYE == TW_TYPE_UNDEF)
		TW_TYPE_RENDER_STLYE = TwDefineEnum("TW_TYPE_RENDER_STLYE", d_renderStyleMembers, 3);

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Material", typeName, entity.getName(), &getRenderComponentMaterialDebugEditor, &setRenderComponentMaterialDebugEditor, this);
	Debug::entityDebugWindow->addVariable(&m_renderStyle, TW_TYPE_RENDER_STLYE, "Render Style", typeName, entity.getName());
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
	m_material = AssetManager::getAsset<Material>(materialID);
	if (m_material)
	{
		m_materialID = materialID;
	}
	else
	{
		m_materialID = "";
		m_material = nullptr;
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
