#pragma once
#include "DebugWindow.h"

class Scene;
class Entity;
class Component;

class EntityDebugWindow : public DebugWindow
{
public:
	EntityDebugWindow(std::string windowID, std::string windowLabel);
	~EntityDebugWindow();

	void addEntity(Entity* entity);
	void removeEntity(Scene& scene, Entity* entity);

	void addComponent(Component* component);
	void removeComponent(Component* component);
};

void TW_CALL removeEntityDebugWindow(void* clientData);
void TW_CALL addComponentDebugWindow(void* clientData);
void TW_CALL removeComponentDebugWindow(void* clientData);