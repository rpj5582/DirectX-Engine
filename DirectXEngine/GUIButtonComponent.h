#pragma once
#include "GUISpriteComponent.h"

#include <functional>

class GUIButtonComponent : public GUISpriteComponent
{
public:
	GUIButtonComponent(Entity& entity);
	~GUIButtonComponent();

	virtual void init() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	template<typename T>
	void setOnClickCallback(T& onClick);

	DirectX::SpriteFont* getFont() const;
	void setFont(DirectX::SpriteFont* font);

	std::string getText() const;
	void setText(std::string text);

	DirectX::XMFLOAT4 getTextColor() const;
	void setTextColor(DirectX::XMFLOAT4 color);

protected:
	void onMouseDown(WPARAM buttonState, int x, int y) override;
	void onMouseUp(WPARAM buttonState, int x, int y) override;

private:
	std::function<void()> m_onClick;
	bool m_isClicking;

	DirectX::SpriteFont* m_font;
	std::string m_text;
	DirectX::XMFLOAT4 m_textColor;
};

template<typename T>
inline void GUIButtonComponent::setOnClickCallback(T& onClick)
{
	m_onClick = onClick;
}
