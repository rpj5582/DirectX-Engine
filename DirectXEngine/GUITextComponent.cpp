#include "GUITextComponent.h"

using namespace DirectX;

GUITextComponent::GUITextComponent(Scene* scene, unsigned int entity) : GUIComponent(scene, entity)
{
	m_font = nullptr;
	m_text = "";
}

GUITextComponent::~GUITextComponent()
{
}

void GUITextComponent::init()
{
	m_font = AssetManager::getFont("Arial");
}

void GUITextComponent::update(float deltaTime, float totalTime)
{
}

SpriteFont* GUITextComponent::getFont() const
{
	return m_font;
}

void GUITextComponent::setFont(SpriteFont* font)
{
	m_font = font;
}

std::string GUITextComponent::getText() const
{
	return m_text;
}

void GUITextComponent::setText(std::string text)
{
	m_text = text;
}
