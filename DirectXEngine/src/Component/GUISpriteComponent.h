#pragma once
#include "GUIComponent.h"

class GUISpriteComponent : public GUIComponent
{
public:
	GUISpriteComponent(Entity& entity);
	virtual ~GUISpriteComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	Texture* getTexture() const;
	void setTexture(Texture* texture);

protected:
	Texture* m_texture;
};

void debugGUISpriteComponentSetTexture(Component* component, const void* value);