#include "Rigidbody.h"

#include "../Scene/Scene.h"

#include "Transform.h"
#include "GUITextComponent.h"

#include "../Input.h"

using namespace DirectX;

Rigidbody::Rigidbody(Entity& entity) : Component(entity)
{
	m_velocity = XMFLOAT3();
	m_totalForce = XMFLOAT3();
	m_invMass = 0;
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::init()
{
	Component::init();

	m_invMass = 1;
}

void Rigidbody::update(float deltaTime, float totalTime)
{
	Component::update(deltaTime, totalTime);

	if (Input::isKeyPressed(Keyboard::R))
	{
		Transform* transform = entity.getComponent<Transform>();
		transform->setLocalPosition(XMFLOAT3(0.0f, 1.0f, -20.0f));
		m_velocity = XMFLOAT3();
		m_totalForce = XMFLOAT3();
	}

	if (Input::isKeyPressed(Keyboard::Space))
	{
		addForce(XMFLOAT3(0.0f, 150.0f, 150.0f));
	}

	addForce(XMFLOAT3(0.0f, -0.25f, 0.0f)); // Gravity

	resolvePhysics();
}

float Rigidbody::getMass() const
{
	return 1.0f / m_invMass;
}

void Rigidbody::setMass(float mass)
{
	m_invMass = 1.0f / mass;
}

void Rigidbody::addForce(DirectX::XMFLOAT3 force)
{
	XMVECTOR totalForce = XMLoadFloat3(&m_totalForce);
	XMVECTOR f = XMLoadFloat3(&force);

	totalForce = XMVectorAdd(totalForce, f);

	XMStoreFloat3(&m_totalForce, totalForce);
}

void Rigidbody::resolvePhysics()
{
	float physDeltaTime = 0.016f;

	Transform* transform = entity.getComponent<Transform>();

	XMVECTOR time = XMVectorSet(physDeltaTime, physDeltaTime, physDeltaTime, physDeltaTime);

	XMVECTOR acceleration = XMVectorMultiply(XMLoadFloat3(&m_totalForce), XMVectorSet(m_invMass, m_invMass, m_invMass, m_invMass));

	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	velocity = XMVectorMultiplyAdd(acceleration, time, velocity);

	XMStoreFloat3(&m_velocity, velocity);

	XMFLOAT3 delta;
	XMStoreFloat3(&delta, XMVectorMultiply(velocity, time));
	transform->move(delta);

	m_totalForce = XMFLOAT3();

	// ------HACK
	// Since there's no collision detection, for right now just don't let the rigidbody go below the floor (1)
	XMFLOAT3 position = transform->getPosition();
	if (position.y < 1.0f)
	{
		transform->setLocalPosition(XMFLOAT3(position.x, 1.0f, position.z));
		m_velocity.y = 0;
	}
	// ------HACK
}