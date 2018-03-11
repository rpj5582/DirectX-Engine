#pragma once
#include "Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody(Entity& entity);
	~Rigidbody();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	float getMass() const;
	void setMass(float mass);
	void addForce(DirectX::XMFLOAT3 force);

private:
	void resolvePhysics();

	DirectX::XMFLOAT3 m_velocity;
	DirectX::XMFLOAT3 m_totalForce;
	float m_invMass;
};