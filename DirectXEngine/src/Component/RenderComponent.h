#pragma once
#include "Component.h"

enum RenderStyle
{
	SOLID,
	WIREFRAME,
	SOLID_WIREFRAME
};

class RenderComponent : public Component
{
public:
	RenderComponent(Entity& entity);
	virtual ~RenderComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	Material* getMaterial() const;
	void setMaterial(Material* material);

	RenderStyle getRenderStyle() const;
	void setRenderStyle(RenderStyle renderStyle);

	DirectX::XMFLOAT4 getWireframeColor() const;
	void setWireframeColor(DirectX::XMFLOAT4 color);

private:
	Material* m_material;

	RenderStyle m_renderStyle;
	DirectX::XMFLOAT4 m_wireColor;

	static std::vector<std::pair<RenderStyle, std::string>> d_renderStyleEnumStrings;
};

void debugRenderComponentSetMaterial(Component* component, const void* value);
