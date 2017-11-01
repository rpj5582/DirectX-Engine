#pragma once
#include "GUIComponent.h"

class GUITextComponent: public GUIComponent
{
public:
	GUITextComponent(Entity& entity);
	~GUITextComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	Font* getFont() const;
	std::string getFontID() const;
	void setFont(std::string fontID);

	std::string getText() const;
	void setText(std::string text);

private:
	Font* m_font;
	std::string m_fontID;

	std::string m_text;
};

void TW_CALL getGUITextComponentFontDebugEditor(void* value, void* clientData);
void TW_CALL setGUITextComponentFontDebugEditor(const void* value, void* clientData);