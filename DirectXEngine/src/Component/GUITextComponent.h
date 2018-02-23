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
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	Font* getFont() const;
	void setFont(Font* font);

	std::string getText() const;
	void setText(std::string text);

private:
	Font* m_font;

	std::string m_text;
};

void debugGUITextComponentSetFont(Component* component, const void* value);