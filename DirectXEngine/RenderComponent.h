#pragma once
#include "Component.h"

#include "AssetManager.h"

enum RenderStyle
{
	SOLID,
	WIREFRAME,
	SOLID_WIREFRAME
};

class RenderComponent : public Component
{
public:
	RenderComponent(Scene* scene, unsigned int entity);
	virtual ~RenderComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	Material* getMaterial() const;
	void setMaterial(Material* material);

	RenderStyle getRenderStyle() const;
	void setRenderStyle(RenderStyle renderStyle);

private:
	Material* m_material;
	RenderStyle m_renderStyle;
};