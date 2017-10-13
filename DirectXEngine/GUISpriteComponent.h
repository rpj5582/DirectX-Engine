#pragma once
#include "GUIComponent.h"

class GUISpriteComponent : public GUIComponent
{
public:
	GUISpriteComponent(Entity& entity);
	virtual ~GUISpriteComponent();

	virtual void init() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	ID3D11ShaderResourceView* getTextureSRV() const;
	void setTexture(std::string textureID);

protected:
	ID3D11ShaderResourceView* m_textureSRV;
	std::string m_textureID;
};