#pragma once
#include "GUISpriteComponent.h"

#include <functional>

class GUIButtonComponent : public GUISpriteComponent
{
public:
	GUIButtonComponent(Entity& entity);
	~GUIButtonComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void update(float deltaTime, float totalTime) override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
	virtual void draw(DirectX::SpriteBatch& spriteBatch) const override;

	template<typename T>
	void setOnClickCallback(T& onClick);

	Font* getFont() const;
	void setFont(Font* font);

	std::string getText() const;
	void setText(std::string text);

	DirectX::XMFLOAT4 getTextColor() const;
	void setTextColor(DirectX::XMFLOAT4 color);

private:
	std::function<void()> m_onClick;

	Font* m_font;

	std::string m_text;
	DirectX::XMFLOAT4 m_textColor;
};

template<typename T>
inline void GUIButtonComponent::setOnClickCallback(T& onClick)
{
	m_onClick = onClick;
}

void debugGUIButtonComponentSetFont(Component* component, const void* value);
