#include "RenderComponent.h"

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
	m_material = AssetManager::getMaterial("default");
}

void RenderComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator material = dataObject.FindMember("material");
	if (material != dataObject.MemberEnd())
	{
		m_material = AssetManager::getMaterial(material->value.GetString());
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
			Output::Warning("Invalid render style " + std::string(renderStyleString) + " on RenderComponent of entity " + entity.getName() + ". Keeping default of SOLID.");
			break;
		}
	}
}

Material* RenderComponent::getMaterial() const
{
	return m_material;
}

void RenderComponent::setMaterial(Material* material)
{
	m_material = material;
}

RenderStyle RenderComponent::getRenderStyle() const
{
	return m_renderStyle;
}

void RenderComponent::setRenderStyle(RenderStyle renderStyle)
{
	m_renderStyle = renderStyle;
}
