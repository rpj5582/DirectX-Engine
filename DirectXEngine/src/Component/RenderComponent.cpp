#include "RenderComponent.h"

using namespace DirectX;

RenderComponent::RenderComponent(Entity& entity) : Component(entity)
{
	m_material = nullptr;
	m_renderStyle = SOLID;
	m_wireColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::init()
{
	Component::init();

	if(!m_material)
		setMaterial(AssetManager::getAsset<Material>(DEFAULT_MATERIAL));
}

void RenderComponent::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddMaterial("Material", &m_material, nullptr, &debugRenderComponentSetMaterial);

	unsigned int renderStyleHash = Debug::registerEnum<RenderStyle>("Solid\0Wireframe\0Solid Wireframe\0\0");
	debugAddEnum("Render Style", &m_renderStyle, renderStyleHash);

	debugAddColor("Wireframe Color", &m_wireColor);
}

void RenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator material = dataObject.FindMember("material");
	if (material != dataObject.MemberEnd())
	{
		setMaterial(AssetManager::getAsset<Material>(material->value.GetString()));
	}

	rapidjson::Value::MemberIterator renderStyle = dataObject.FindMember("renderStyle");
	if (renderStyle != dataObject.MemberEnd())
	{
		const char* renderStyleString = renderStyle->value.GetString();
		switch (Util::stringHash(renderStyleString))
		{
		case Util::stringHash("solid"):
		{
			m_renderStyle = SOLID;
			break;
		}

		case Util::stringHash("wireframe"):
		{
			m_renderStyle = WIREFRAME;
			break;
		}

		case Util::stringHash("solid-wireframe"):
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

void RenderComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("material");
	writer.String(m_material->getAssetID().c_str());

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

void RenderComponent::setMaterial(Material* material)
{
	if (material)
		m_material = material;
	else
		m_material = AssetManager::getAsset<Material>(DEFAULT_MATERIAL);
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

void debugRenderComponentSetMaterial(Component* component, const void* value)
{
	Material* material = *static_cast<Material*const*>(value);
	static_cast<RenderComponent*>(component)->setMaterial(material);
}
