#pragma once
#include "DebugWindow.h"

class Entity;
class Component;

class EntityDebugWindow : public DebugWindow
{
public:
	EntityDebugWindow(std::string windowID, std::string windowLabel);
	~EntityDebugWindow();

	void setupControls(Scene* scene) override;

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

	void addComponent(Component* component);
	void removeComponent(Component* component);
};

void TW_CALL addEntityDebugEditor(void* clientData);
void TW_CALL removeEntityDebugEditor(void* clientData);
void TW_CALL addComponentDebugEditor(void* clientData);
void TW_CALL removeComponentDebugEditor(void* clientData);