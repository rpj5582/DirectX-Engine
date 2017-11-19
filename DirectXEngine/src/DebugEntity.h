#pragma once

class Entity;
class GUITransform;
class GUIDebugSpriteComponent;

class DebugEntity
{
public:
	DebugEntity(Entity& entity);
	~DebugEntity();

	GUITransform* getGUITransform() const;
	GUIDebugSpriteComponent* getGUIDebugSpriteComponent() const;

	bool enabled;

private:
	GUITransform* m_guiTransform;
	GUIDebugSpriteComponent* m_guiDebugSpriteComponent;
};
