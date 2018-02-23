#pragma once
#include "IDebugEditor.h"

class Entity;

class DebugEntityList : public IDebugEditor
{
public:
	DebugEntityList();
	~DebugEntityList();

	unsigned int getSelectedEntity() const;

	void draw() override;

private:
	void handleEntity(Entity& entity);

	static unsigned int m_selectedEntityID;
};