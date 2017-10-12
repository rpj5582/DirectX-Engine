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
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;

	Material* getMaterial() const;
	void setMaterial(Material* material);

	RenderStyle getRenderStyle() const;
	void setRenderStyle(RenderStyle renderStyle);

private:
	Material* m_material;
	RenderStyle m_renderStyle;
};