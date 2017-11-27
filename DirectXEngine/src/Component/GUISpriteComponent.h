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
	std::string getTextureID() const;
	void setTexture(std::string textureID);

protected:
	Texture* m_texture;
	std::string m_textureID;
};

void TW_CALL getGUISpriteComponentTextureDebugEditor(void* value, void* clientData);
void TW_CALL setGUISpriteComponentTextureDebugEditor(const void* value, void* clientData);