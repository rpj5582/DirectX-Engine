#pragma once
#include "GUIComponent.h"

class GUISpriteComponent : public GUIComponent
{
public:
	GUISpriteComponent(Scene* scene, unsigned int entity);
	~GUISpriteComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	ID3D11ShaderResourceView* getTextureSRV() const;
	void setTextureSRV(ID3D11ShaderResourceView* textureSRV);

private:
	ID3D11ShaderResourceView* m_textureSRV;
};