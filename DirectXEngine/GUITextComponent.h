#pragma once
#include "GUIComponent.h"

class GUITextComponent: public GUIComponent
{
public:
	GUITextComponent(Scene* scene, Entity entity);
	~GUITextComponent();

	virtual void init() override;
	virtual void draw(Scene* scene, DirectX::SpriteBatch* spriteBatch) const override;

	DirectX::SpriteFont* getFont() const;
	void setFont(DirectX::SpriteFont* font);

	std::string getText() const;
	void setText(std::string text);

private:
	DirectX::SpriteFont* m_font;
	std::string m_text;
};