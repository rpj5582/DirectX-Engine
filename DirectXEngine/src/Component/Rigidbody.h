#pragma once
#include "IPhysicsBody.h"

class Transform;

class Rigidbody : public IPhysicsBody
{
public:
	Rigidbody(Entity& entity);
	~Rigidbody();

	void init() override;
	void initDebugVariables() override;
	void update(float deltaTime, float totalTime) override;
	void loadFromJSON(rapidjson::Value& dataObject) override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	void integrateForces() override;
	void integrateVelocity() override;

	BodyData* getClosestBodyData(DirectX::XMFLOAT3 point, unsigned int* i = nullptr, unsigned int* j = nullptr, unsigned int* k = nullptr) override;

	void updateVisual() override;

	float getMass() const;
	float getInverseMass() const;
	void setMass(float mass);

	DirectX::XMFLOAT3X3 getInertia() const;
	DirectX::XMFLOAT3X3 getInverseInertia() const;
	void setInertia(DirectX::XMFLOAT3X3 inertia);

	float getRestitution() const;
	void setRestitution(float restitution);

	float getGravityScale() const;
	void setGravityScale(float scale);

	float getSurfaceFriction() const;

	DirectX::XMFLOAT3 getVelocity() const;
	void setVelocity(DirectX::XMFLOAT3 velocity);

	DirectX::XMFLOAT3 getAngularVelocity() const;
	void setAngularVelocity(DirectX::XMFLOAT3 angularVelocity);

	void applyForce(DirectX::XMFLOAT3 force);
	void applyTorque(DirectX::XMFLOAT3 force, DirectX::XMFLOAT3 point);

private:
	void calculateInertiaTensor();

	BodyData m_bodyData;

	float m_gravityScale;
	float m_friction;
	float m_drag;
	float m_angularDrag;

	Transform* transform;
};

void debugRigidbodyGetMass(const Component* component, void* value);
void debugRigidbodySetMass(Component* component, const void* value);

void debugRigidbodyGetRestitution(const Component* component, void* value);
void debugRigidbodySetRestitution(Component* component, const void* value);

void debugRigidbodyGetGravityScale(const Component* component, void* value);
void debugRigidbodySetGravityScale(Component* component, const void* value);
