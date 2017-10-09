#pragma once
#include "Component.h"

class GUIComponent : public Component
{
public:
	GUIComponent(Scene* scene, unsigned int entity);
	virtual ~GUIComponent();

	virtual void init() override;

	DirectX::XMFLOAT4 getColor() const;
	void setColor(DirectX::XMFLOAT4 color);

private:
	DirectX::XMFLOAT4 m_color;
};