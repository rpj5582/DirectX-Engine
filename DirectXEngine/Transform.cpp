#include "Transform.h"

using namespace DirectX;

Transform::Transform(Scene* scene, unsigned int entity) : Component(scene, entity)
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMStoreFloat4x4(&m_translationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_rotationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_scaleMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::init()
{
}

void Transform::update(float deltaTime, float totalTime)
{
}

const XMFLOAT3 Transform::getPosition() const
{
	return m_position;
}

const XMFLOAT3 Transform::getRotation() const
{
	return m_rotation;
}

const XMFLOAT3 Transform::getScale() const
{
	return m_scale;
}

const XMMATRIX Transform::getTranslationMatrix() const
{
	return XMLoadFloat4x4(&m_translationMatrix);
}

const XMMATRIX Transform::getRotationMatrix() const
{
	return XMLoadFloat4x4(&m_rotationMatrix);
}

const XMMATRIX Transform::getScaleMatrix() const
{
	return XMLoadFloat4x4(&m_scaleMatrix);
}

const XMMATRIX Transform::getWorldMatrix() const
{
	return XMLoadFloat4x4(&m_worldMatrix);
}

const XMFLOAT3 Transform::getRight() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 right;
	XMStoreFloat3(&right, v);
	return right;
}

const XMFLOAT3 Transform::getUp() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 up;
	XMStoreFloat3(&up, v);
	return up;
}

const XMFLOAT3 Transform::getForward() const
{
	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), getRotationMatrix());

	XMFLOAT3 forward;
	XMStoreFloat3(&forward, v);
	return forward;
}

void Transform::calcTranslationMatrix()
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMStoreFloat4x4(&m_translationMatrix, XMMatrixTranslationFromVector(position));

	calcWorldMatrix();
}

void Transform::calcRotationMatrix()
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMStoreFloat4x4(&m_rotationMatrix, XMMatrixRotationRollPitchYawFromVector(rotation));

	calcWorldMatrix();
}

void Transform::calcScaleMatrix()
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	XMStoreFloat4x4(&m_scaleMatrix, XMMatrixScalingFromVector(scale));

	calcWorldMatrix();
}

void Transform::calcWorldMatrix()
{
	XMMATRIX translation = XMLoadFloat4x4(&m_translationMatrix);
	XMMATRIX rotation = XMLoadFloat4x4(&m_rotationMatrix);
	XMMATRIX scale = XMLoadFloat4x4(&m_scaleMatrix);

	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(scale, rotation), translation);
	XMStoreFloat4x4(&m_worldMatrix, world);
}

void Transform::move(XMFLOAT3 delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveX(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveY(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveZ(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveLocal(DirectX::XMFLOAT3 delta)
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

void Transform::moveLocalX(float delta)
{
	XMFLOAT3 rightFloat3 = getRight();

	XMVECTOR right = XMLoadFloat3(&rightFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(right, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveLocalY(float delta)
{
	XMFLOAT3 upFloat3 = getUp();

	XMVECTOR up = XMLoadFloat3(&upFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(up, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::moveLocalZ(float delta)
{
	XMFLOAT3 forwardFloat3 = getForward();

	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR position = XMLoadFloat3(&m_position);
	position = XMVectorAdd(position, XMVectorScale(forward, delta));
	XMStoreFloat3(&m_position, position);

	calcTranslationMatrix();
}

void Transform::rotateLocal(DirectX::XMFLOAT3 rot)
{
	XMFLOAT3 rotDegrees = XMFLOAT3(XMConvertToRadians(rot.x), XMConvertToRadians(rot.y), XMConvertToRadians(rot.z));
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR rotVector = XMLoadFloat3(&rotDegrees);
	rotation = XMVectorAdd(rotation, rotVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Transform::rotateLocalX(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(XMConvertToRadians(angle), 0.0f, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Transform::rotateLocalY(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, XMConvertToRadians(angle), 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Transform::rotateLocalZ(float angle)
{
	XMVECTOR rotation = XMLoadFloat3(&m_rotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, 0.0f, XMConvertToRadians(angle), 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_rotation, rotation);

	calcRotationMatrix();
}

void Transform::scale(DirectX::XMFLOAT3 delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Transform::scaleX(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Transform::scaleY(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}

void Transform::scaleZ(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_scale, scale);

	calcScaleMatrix();
}
