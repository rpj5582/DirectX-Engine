#include "Rigidbody.h"

#include "Transform.h"

#include "../Input.h"

using namespace DirectX;

static float physDeltaTime = 0.016f;

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

	transform = entity.getComponent<Transform>();
	m_invMass = 1;
}

void Rigidbody::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddFloat("Mass", &m_invMass, &debugRigidbodyGetMass, &debugRigidbodySetMass);
	debugAddFloat("Restitution", &m_restitution, &debugRigidbodyGetRestitution, &debugRigidbodySetRestitution);
}

void Rigidbody::update(float deltaTime, float totalTime)
{
	Component::update(deltaTime, totalTime);

	applyForce(XMFLOAT3(0.0f, -9.81f, 0.0f)); // Gravity
}

void Rigidbody::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator mass = dataObject.FindMember("mass");
	if (mass != dataObject.MemberEnd())
	{
		setMass(mass->value.GetFloat());
	}

	rapidjson::Value::MemberIterator restitution = dataObject.FindMember("restitution");
	if (restitution != dataObject.MemberEnd())
	{
		setRestitution(restitution->value.GetFloat());
	}
}

void Rigidbody::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("mass");
	writer.Double(getMass());

	writer.Key("restitution");
	writer.Double(getRestitution());
}

void Rigidbody::integrateForces()
{
	XMVECTOR linearMomentum = XMVectorScale(XMLoadFloat3(&m_totalForce), physDeltaTime);

	XMVECTOR v = XMVectorScale(linearMomentum, m_invMass);

	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMStoreFloat3(&m_velocity, XMVectorAdd(velocity, v));

	m_totalForce = XMFLOAT3();
}

void Rigidbody::integrateVelocity()
{
	XMVECTOR velocity = XMLoadFloat3(&m_velocity);

	XMFLOAT3 delta;
	XMStoreFloat3(&delta, XMVectorScale(velocity, physDeltaTime));
	if (transform) transform->move(delta);
}

float Rigidbody::getMass() const
{
	if (m_invMass > 0)
		return 1.0f / m_invMass;
	else
		return 0.0f;
}

float Rigidbody::getInverseMass() const
{
	return m_invMass;
}

void Rigidbody::setMass(float mass)
{
	if (mass > 0)
		m_invMass = 1.0f / mass;
	else
		m_invMass = 0.0f;
}

float Rigidbody::getRestitution() const
{
	return m_restitution;
}

void Rigidbody::setRestitution(float restitution)
{
	m_restitution = max(0, min(restitution, 1));
}

DirectX::XMFLOAT3 Rigidbody::getVelocity() const
{
	return m_velocity;
}

void Rigidbody::applyForce(DirectX::XMFLOAT3 force)
{
	XMVECTOR forceVec = XMLoadFloat3(&force);
	XMVECTOR totalForce = XMLoadFloat3(&m_totalForce);
	totalForce = XMVectorAdd(forceVec, totalForce);

	XMStoreFloat3(&m_totalForce, totalForce);
}

void Rigidbody::applyImpulse(DirectX::XMFLOAT3 impulse)
{
	XMVECTOR impulseVec = XMLoadFloat3(&impulse);
	XMVECTOR v = XMVectorScale(impulseVec, m_invMass);

	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMStoreFloat3(&m_velocity, XMVectorAdd(velocity, v));
}

void debugRigidbodyGetMass(const Component* component, void* value)
{
	float mass = static_cast<const Rigidbody*>(component)->getMass();
	*static_cast<float*>(value) = mass;
}

void debugRigidbodySetMass(Component* component, const void* value)
{
	Rigidbody* body = static_cast<Rigidbody*>(component);
	body->setMass(*static_cast<const float*>(value));
}

void debugRigidbodyGetRestitution(const Component* component, void* value)
{
	float restitution = static_cast<const Rigidbody*>(component)->getRestitution();
	*static_cast<float*>(value) = restitution;
}

void debugRigidbodySetRestitution(Component* component, const void* value)
{
	Rigidbody* body = static_cast<Rigidbody*>(component);
	body->setRestitution(*static_cast<const float*>(value));
}
