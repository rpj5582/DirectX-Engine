#include "IPhysicsBody.h"

using namespace DirectX;

BodyData::BodyData(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3X3 inertia, float invMass, float restitution, XMFLOAT3 velocity, XMFLOAT3 angularVelocity)
{
	this->position = position;
	this->rotation = rotation;
	this->velocity = velocity;
	this->angularVelocity = angularVelocity;
	this->inertia = inertia;
	this->totalForce = XMFLOAT3();
	this->totalTorque = XMFLOAT3();
	this->restitution = 0;

	this->invMass = invMass;
	if (this->invMass == 0.0f)
	{
		this->inertia = XMFLOAT3X3();
		this->invInertia = XMFLOAT3X3();
	}
	else
	{
		this->inertia = inertia;

		// Check for inverse of zero matrix
		XMMATRIX invInertiaTensor = XMLoadFloat3x3(&inertia);
		XMVECTOR determinantVec;
		invInertiaTensor = XMMatrixInverse(&determinantVec, invInertiaTensor);
		float determinant;
		XMStoreFloat(&determinant, determinantVec);
		if (determinant == 0)
		{
			this->invInertia = XMFLOAT3X3();
		}
	}
}

void BodyData::integrateForces()
{
	XMVECTOR linearMomentum = XMVectorScale(XMLoadFloat3(&totalForce), physDeltaTime);
	XMVECTOR v = XMVectorScale(linearMomentum, invMass);

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMStoreFloat3(&velocity, XMVectorAdd(velocityVec, v));


	XMVECTOR angularMomentum = XMVectorScale(XMLoadFloat3(&totalTorque), physDeltaTime);
	XMVECTOR w = XMVector3Transform(angularMomentum, XMLoadFloat3x3(&invInertia));

	XMVECTOR angularVelocityVec = XMLoadFloat3(&angularVelocity);
	XMStoreFloat3(&angularVelocity, XMVectorAdd(angularVelocityVec, w));

	totalForce = XMFLOAT3();
	totalTorque = XMFLOAT3();
}

void BodyData::integrateVelocity()
{
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMVECTOR rotationVec = XMLoadFloat3(&rotation);
	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMVECTOR angularVelocityVec = XMLoadFloat3(&angularVelocity);

	XMStoreFloat3(&position, XMVectorAdd(positionVec, XMVectorScale(velocityVec, physDeltaTime)));
	XMStoreFloat3(&rotation, XMVectorAdd(rotationVec, XMVectorScale(angularVelocityVec, physDeltaTime)));
}

IPhysicsBody::IPhysicsBody(Entity& entity) : Component(entity)
{
}

IPhysicsBody::~IPhysicsBody()
{
	if (!entity.getComponent<IPhysicsBody>())
	{
		if (entity.hasTag(TAG_PHYSICSBODY))
			entity.removeTag(TAG_PHYSICSBODY);
	}
}

void IPhysicsBody::init()
{
	Component::init();

	if (!entity.hasTag(TAG_PHYSICSBODY))
		entity.addTag(TAG_PHYSICSBODY);
}
