#pragma once
#include "GUIComponent.h"

class GUITextComponent: public GUIComponent
{
public:
	GUITextComponent(Entity& entity);
	~GUITextComponent();

	virtual void init() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	DirectX::SpriteFont* getFont() const;
	void setFont(std::string fontID);

	std::string getText() const;
	void setText(std::string text);

private:
	DirectX::SpriteFont* m_font;
	std::string m_fontID;

	std::string m_text;
};