#pragma once
#include "Component.h"

static float physDeltaTime = 0.002f;

struct BodyData
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 angularVelocity;
	DirectX::XMFLOAT3X3 inertia;
	DirectX::XMFLOAT3X3 invInertia;
	DirectX::XMFLOAT3 totalForce;
	DirectX::XMFLOAT3 totalTorque;
	float invMass;
	float restitution;

	BodyData() = default;
	BodyData(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3X3 inertia, float invMass, float restitution,
		DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3(), DirectX::XMFLOAT3 angularVelocity = DirectX::XMFLOAT3());

	void integrateForces();
	void integrateVelocity();
};

class IPhysicsBody : public Component
{
public:
	IPhysicsBody(Entity& entity);
	virtual ~IPhysicsBody();

	virtual void init() override;

	virtual void integrateForces() = 0;
	virtual void integrateVelocity() = 0;
	virtual BodyData* getClosestBodyData(DirectX::XMFLOAT3 point, unsigned int* i = nullptr, unsigned int* j = nullptr, unsigned int* k = nullptr) = 0;
	virtual void updateVisual() = 0;
};