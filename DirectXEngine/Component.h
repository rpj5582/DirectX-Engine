#pragma once

class Entity;

class Component
{
public:
	friend class Entity;

	virtual void init() = 0;
	virtual void update() = 0;

	Entity* getEntity() const;
	
protected:
	Component(Entity* entity);
	~Component();

	Entity* m_entity;
};