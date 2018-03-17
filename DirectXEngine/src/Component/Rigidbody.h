#pragma once
#include "Component.h"

class Transform;

class Rigidbody : public Component
{
public:
	Rigidbody(Entity& entity);
	~Rigidbody();

	void init() override;
	void initDebugVariables() override;
	void update(float deltaTime, float totalTime) override;
	void loadFromJSON(rapidjson::Value& dataObject) override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	void integrateForces();
	void integrateVelocity();

	float getMass() const;
	float getInverseMass() const;
	void setMass(float mass);

	float getRestitution() const;
	void setRestitution(float restitution);

	DirectX::XMFLOAT3 getVelocity() const;

	void applyForce(DirectX::XMFLOAT3 force);
	void applyImpulse(DirectX::XMFLOAT3 impulse);

private:
	Transform* transform;

	DirectX::XMFLOAT3 m_velocity;
	DirectX::XMFLOAT3 m_totalForce;
	float m_invMass;
	float m_restitution;
};

void debugRigidbodyGetMass(const Component* component, void* value);
void debugRigidbodySetMass(Component* component, const void* value);

void debugRigidbodyGetRestitution(const Component* component, void* value);
void debugRigidbodySetRestitution(Component* component, const void* value);
