#pragma once
#include "GUIComponent.h"

class GUISpriteComponent : public GUIComponent
{
public:
	GUISpriteComponent(Scene* scene, Entity entity);
	virtual ~GUISpriteComponent();

	virtual void init() override;
	virtual void draw(Scene* scene, DirectX::SpriteBatch* spriteBatch) const override;

	ID3D11ShaderResourceView* getTextureSRV() const;
	void setTextureSRV(ID3D11ShaderResourceView* textureSRV);

protected:
	ID3D11ShaderResourceView* m_textureSRV;
};