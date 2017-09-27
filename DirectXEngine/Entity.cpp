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

const XMVECTOR Entity::getRight() const
{
	return XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), getRotationMatrix());
}

const XMVECTOR Entity::getUp() const
{
	return XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), getRotationMatrix());
}

const XMVECTOR Entity::getForward() const
{
	return XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), getRotationMatrix());
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
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(getRight(), delta.x));
	position = XMVectorAdd(position, XMVectorScale(getUp(), delta.y));
	position = XMVectorAdd(position, XMVectorScale(getForward(), delta.z));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalX(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(getRight(), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalY(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(getUp(), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::moveLocalZ(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(getForward(), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Entity::rotateLocal(DirectX::XMFLOAT3 rot)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR rotVector = XMLoadFloat3(&rot);
	rotation = XMVectorAdd(rotation, rotVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalX(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(angle, 0.0f, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalY(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, angle, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Entity::rotateLocalZ(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, 0.0f, angle, 0.0f);
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
