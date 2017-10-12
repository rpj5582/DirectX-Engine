#pragma once
#include "Component.h"

class GUIComponent : public Component
{
public:
	GUIComponent(Entity& entity);
	virtual ~GUIComponent();

	virtual void init() override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const = 0;

	DirectX::XMFLOAT4 getColor() const;
	void setColor(DirectX::XMFLOAT4 color);

protected:
	DirectX::XMFLOAT4 m_color;
};