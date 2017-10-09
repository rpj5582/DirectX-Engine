#pragma once
#include "GUIComponent.h"

class GUITextComponent: public GUIComponent
{
public:
	GUITextComponent(Scene* scene, unsigned int entity);
	~GUITextComponent();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	DirectX::SpriteFont* getFont() const;
	void setFont(DirectX::SpriteFont* font);

	std::string getText() const;
	void setText(std::string text);

private:
	DirectX::SpriteFont* m_font;
	std::string m_text;
};