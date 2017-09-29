#include "Entity.h"

using namespace DirectX;

Entity::Entity(Mesh* mesh, Material* material)
{
	m_mesh = mesh;
	m_material = material;

	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMStoreFloat4x4(&m_translationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_rotationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_scaleMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldMatrixInverseTranspose, XMMatrixIdentity());
}

Entity::~Entity()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		delete m_components[i];
	}
	m_components.clear();
}

Mesh* Entity::getMesh() const
{
	return m_mesh;
}

Material* Entity::getMaterial() const
{
	return m_material;
}

const XMFLOAT3 Entity::getPosition() const
{
	return m_position;
}

const XMFLOAT3 Entity::getRotation() const
{
	return m_rotation;
}

const XMFLOAT3 Entity::getScale() const
{
	return m_scale;
}

const XMMATRIX Entity::getTranslationMatrix() const
{
	return XMLoadFloat4x4(&m_translationMatrix);
}

const XMMATRIX Entity::getRotationMatrix() const
{
	return XMLoadFloat4x4(&m_rotationMatrix);
}

const XMMATRIX Entity::getScaleMatrix() const
{
	return XMLoadFloat4x4(&m_scaleMatrix);
}

const XMMATRIX Entity::getWorldMatrix() const
{
	return XMLoadFloat4x4(&m_worldMatrix);
}

const DirectX::XMMATRIX Entity::getWorldMatrixInverseTranspose() const
{
	return XMLoadFloat4x4(&m_worldMatrixInverseTranspose);
}

const XMFLOAT3 Entity::getRight() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 right;
	XMStoreFloat3(&right, v);
	return right;
}

const XMFLOAT3 Entity::getUp() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 up;
	XMStoreFloat3(&up, v);
	return up;
}

const XMFLOAT3 Entity::getForward() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 forward;
	XMStoreFloat3(&forward, v);
	return forward;
}

void Entity::calcTranslationMatrix()
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMStoreFloat4x4(&m_translationMatrix, XMMatrixTranslationFromVector(position));

	calcWorldMatrix();
}

void Entity::calcRotationMatrix()
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMStoreFloat4x4(&m_rotationMatrix, XMMatrixRotationRollPitchYawFromVector(rotation));

	calcWorldMatrix();
}

void Entity::calcScaleMatrix()
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	XMStoreFloat4x4(&m_scaleMatrix, XMMatrixScalingFromVector(scale));

	calcWorldMatrix();
}

void Entity::calcWorldMatrix()
{
	XMMATRIX translation = XMLoadFloat4x4(&m_translationMatrix);
	XMMATRIX rotation = XMLoadFloat4x4(&m_rotationMatrix);
	XMMATRIX scale = XMLoadFloat4x4(&m_scaleMatrix);

	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(scale, rotation), translation);
	XMStoreFloat4x4(&m_worldMatrix, world);

	XMMATRIX worldInverseTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
	XMStoreFloat4x4(&m_worldMatrixInverseTranspose, worldInverseTranspose);
}

void Entity::move(XMFLOAT3 delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveX(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveY(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveZ(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocal(DirectX::XMFLOAT3 delta)
{
	XMFLOAT3 rightFloat3 = getRight();
	XMFLOAT3 upFloat3 = getUp();
	XMFLOAT3 forwardFloat3 = getForward();

	XMVECTOR right = XMLoadFloat3(&rightFloat3);
	XMVECTOR up = XMLoadFloat3(&upFloat3);
	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);

	position = XMVectorAdd(position, XMVectorScale(right, delta.x));
	position = XMVectorAdd(position, XMVectorScale(up, delta.y));
	position = XMVectorAdd(position, XMVectorScale(forward, delta.z));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalX(float delta)
{
	XMFLOAT3 rightFloat3 = getRight();

	XMVECTOR right = XMLoadFloat3(&rightFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(right, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalY(float delta)
{
	XMFLOAT3 upFloat3 = getUp();

	XMVECTOR up = XMLoadFloat3(&upFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(up, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalZ(float delta)
{
	XMFLOAT3 forwardFloat3 = getForward();

	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(forward, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::rotateLocal(DirectX::XMFLOAT3 rot)
{
	XMFLOAT3 rotDegrees = XMFLOAT3(XMConvertToRadians(rot.x), XMConvertToRadians(rot.y), XMConvertToRadians(rot.z));
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR rotVector = XMLoadFloat3(&rotDegrees);
	rotation = XMVectorAdd(rotation, rotVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalX(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(XMConvertToRadians(angle), 0.0f, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalY(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, XMConvertToRadians(angle), 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalZ(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, 0.0f, XMConvertToRadians(angle), 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::scale(DirectX::XMFLOAT3 delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Entity::scaleX(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Entity::scaleY(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Entity::scaleZ(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}
