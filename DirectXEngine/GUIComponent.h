#pragma once
#include "Component.h"

class GUIComponent : public Component
{
public:
	GUIComponent(Scene* scene, unsigned int entity);
	~GUIComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	ID3D11ShaderResourceView* getTextureSRV() const;
	void setTextureSRV(ID3D11ShaderResourceView* textureSRV);

private:
	ID3D11ShaderResourceView* m_textureSRV;
};