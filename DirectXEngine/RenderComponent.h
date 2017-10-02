#pragma once
#include "Component.h"

#include "AssetManager.h"

class RenderComponent : public Component
{
public:
	RenderComponent(Scene* scene, unsigned int entity);
	virtual ~RenderComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	Material* getMaterial() const;
	void changeMaterial(Material* material);

private:
	Material* m_material;
};