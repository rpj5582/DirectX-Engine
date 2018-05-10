#include "Rigidbody.h"

#include "Transform.h"

using namespace DirectX;

Rigidbody::Rigidbody(Entity& entity) : IPhysicsBody(entity)
{
	m_gravityScale = 1.0f;
	m_friction = 0.0f;
	m_drag = 0.1f;
	m_angularDrag = 0.1f;
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::init()
{
	IPhysicsBody::init();

	calculateInertiaTensor();

	transform = entity.getComponent<Transform>();
	
	m_bodyData.position = transform->getPosition();
	m_bodyData.rotation = transform->getLocalRotation();
}

void Rigidbody::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddFloat("Mass", &m_bodyData.invMass, &debugRigidbodyGetMass, &debugRigidbodySetMass);
	debugAddFloat("Restitution", &m_bodyData.restitution, &debugRigidbodyGetRestitution, &debugRigidbodySetRestitution);
	debugAddFloat("Gravity Scale", &m_gravityScale, &debugRigidbodyGetGravityScale, &debugRigidbodySetGravityScale);
	debugAddFloat("Surface Friction", &m_friction);
	debugAddFloat("Drag", &m_drag);
	debugAddFloat("Angular Drag", &m_angularDrag);
}

void Rigidbody::update(float deltaTime, float totalTime)
{
	Component::update(deltaTime, totalTime);

	// Update the rigidbodies position and rotation with the transform's position and rotation
	m_bodyData.position = transform->getPosition();
	m_bodyData.rotation = transform->getLocalRotation();

	// Gravity
	applyForce(XMFLOAT3(0.0f, -9.81f * m_gravityScale, 0.0f));
	
	// Linear drag (not friction, models air resistance)
	XMVECTOR velocity = XMLoadFloat3(&m_bodyData.velocity);

	XMVECTOR dragDirection = XMVector3Normalize(XMVectorNegate(velocity));
	float dragMagnitude;
	XMStoreFloat(&dragMagnitude, XMVector3Length(velocity));

	dragMagnitude = min(dragMagnitude, m_drag);

	XMVECTOR dragForceVec = XMVectorScale(dragDirection, dragMagnitude);
	XMFLOAT3 dragForce;
	XMStoreFloat3(&dragForce, dragForceVec);
	applyForce(dragForce);

	//// Angular drag
	//XMVECTOR angularVelocity = XMLoadFloat3(&m_angularVelocity);

	//dragDirection = XMVector3Normalize(XMVectorNegate(angularVelocity));
	//XMStoreFloat(&dragMagnitude, XMVector3Length(angularVelocity));

	//dragMagnitude = min(dragMagnitude, m_angularDrag);

	//XMVECTOR dragTorqueVec = XMVectorScale(dragDirection, dragMagnitude);
	//XMFLOAT3 dragTorque;
	//XMStoreFloat3(&dragTorque, dragTorqueVec);
	//applyTorque(dragTorque);
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

	rapidjson::Value::MemberIterator gravityScale = dataObject.FindMember("gravityScale");
	if (gravityScale != dataObject.MemberEnd())
	{
		setGravityScale(gravityScale->value.GetFloat());
	}
}

void Rigidbody::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("mass");
	writer.Double(getMass());

	writer.Key("restitution");
	writer.Double(getRestitution());

	writer.Key("gravityScale");
	writer.Double(getGravityScale());
}

void Rigidbody::integrateForces()
{
	m_bodyData.integrateForces();
}

void Rigidbody::integrateVelocity()
{
	m_bodyData.integrateVelocity();
	updateVisual();
}

BodyData* Rigidbody::getClosestBodyData(DirectX::XMFLOAT3 point, unsigned int* i, unsigned int* j, unsigned int* k)
{
	return &m_bodyData;
}

void Rigidbody::updateVisual()
{
	transform->setLocalPosition(m_bodyData.position);
	transform->setLocalRotationRadians(m_bodyData.rotation);
}

float Rigidbody::getMass() const
{
	if (m_bodyData.invMass > 0)
		return 1.0f / m_bodyData.invMass;
	else
		return 0.0f;
}

float Rigidbody::getInverseMass() const
{
	return m_bodyData.invMass;
}

void Rigidbody::setMass(float mass)
{
	if (mass > 0)
		m_bodyData.invMass = 1.0f / mass;
	else
		m_bodyData.invMass = 0.0f;
}

XMFLOAT3X3 Rigidbody::getInertia() const
{
	return m_bodyData.inertia;
}

XMFLOAT3X3 Rigidbody::getInverseInertia() const
{
	return m_bodyData.invInertia;
}

void Rigidbody::setInertia(XMFLOAT3X3 inertia)
{
	m_bodyData.inertia = inertia;

	XMMATRIX inertiaTensor = XMLoadFloat3x3(&m_bodyData.inertia);

	float determinant;
	XMVECTOR determinantVec;
	XMMATRIX invInertiaTensor = XMMatrixInverse(&determinantVec, inertiaTensor);
	XMStoreFloat(&determinant, determinantVec);
	if (determinant == 0.0f)
	{
		m_bodyData.invInertia = XMFLOAT3X3();
	}
	else
		XMStoreFloat3x3(&m_bodyData.invInertia, invInertiaTensor);
}

float Rigidbody::getRestitution() const
{
	return m_bodyData.restitution;
}

void Rigidbody::setRestitution(float restitution)
{
	m_bodyData.restitution = max(0, min(restitution, 1));
}

float Rigidbody::getGravityScale() const
{
	return m_gravityScale;
}

void Rigidbody::setGravityScale(float scale)
{
	m_gravityScale = scale;
}

float Rigidbody::getSurfaceFriction() const
{
	return m_friction;
}

DirectX::XMFLOAT3 Rigidbody::getVelocity() const
{
	return m_bodyData.velocity;
}

void Rigidbody::setVelocity(DirectX::XMFLOAT3 velocity)
{
	m_bodyData.velocity = velocity;
}

DirectX::XMFLOAT3 Rigidbody::getAngularVelocity() const
{
	return m_bodyData.angularVelocity;
}

void Rigidbody::setAngularVelocity(DirectX::XMFLOAT3 angularVelocity)
{
	m_bodyData.angularVelocity = angularVelocity;
}

void Rigidbody::applyForce(DirectX::XMFLOAT3 force)
{
	XMVECTOR forceVec = XMLoadFloat3(&force);
	XMVECTOR totalForce = XMLoadFloat3(&m_bodyData.totalForce);
	totalForce = XMVectorAdd(forceVec, totalForce);

	XMStoreFloat3(&m_bodyData.totalForce, totalForce);
}

void Rigidbody::applyTorque(DirectX::XMFLOAT3 force, DirectX::XMFLOAT3 point)
{
	XMVECTOR forceVec = XMLoadFloat3(&force);
	XMVECTOR pointVec = XMLoadFloat3(&point);
	
	XMVECTOR origin = XMLoadFloat3(&m_bodyData.position);

	XMVECTOR totalTorque = XMLoadFloat3(&m_bodyData.totalTorque);
	totalTorque = XMVectorAdd(XMVector3Cross(forceVec, XMVectorSubtract(pointVec, origin)), totalTorque);

	XMStoreFloat3(&m_bodyData.totalTorque, totalTorque);
}

void Rigidbody::calculateInertiaTensor()
{
	if (m_bodyData.invMass == 0.0f)
	{
		m_bodyData.inertia = XMFLOAT3X3();
		m_bodyData.invInertia = XMFLOAT3X3();
		return;
	}

	// For now, assume the inertia tensor is for a 1x1x1 cube
	float scale = (1.0f / 6.0f) * (1.0f / m_bodyData.invMass);

	XMMATRIX inertiaTensor = XMMatrixScaling(scale, scale, scale);
	XMMATRIX invInertiaTensor = XMMatrixInverse(nullptr, inertiaTensor);

	XMStoreFloat3x3(&m_bodyData.inertia, inertiaTensor);
	XMStoreFloat3x3(&m_bodyData.invInertia, invInertiaTensor);
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

void debugRigidbodyGetGravityScale(const Component* component, void* value)
{
	float gravityScale = static_cast<const Rigidbody*>(component)->getGravityScale();
	*static_cast<float*>(value) = gravityScale;
}

void debugRigidbodySetGravityScale(Component* component, const void* value)
{
	Rigidbody* body = static_cast<Rigidbody*>(component);
	body->setGravityScale(*static_cast<const float*>(value));
}
