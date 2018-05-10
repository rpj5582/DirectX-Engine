#include "Transform.h"

using namespace DirectX;

Transform::Transform(Entity& entity) : Component(entity)
{
	m_localPosition = XMFLOAT3();
	m_localRotation = XMFLOAT3();
	m_localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixIdentity());
	m_isDirty = false;
}

Transform::~Transform()
{
#if defined(DEBUG) || defined(_DEBUG)
	entity.disableDebugIcon();
#endif
}

void Transform::init()
{
	Component::init();

#if defined(DEBUG) || defined(_DEBUG)
	entity.enableDebugIcon();
#endif
}

void Transform::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddVec3("Position", &m_localPosition, nullptr, &debugTransformSetLocalPosition);
	debugAddVec3("Rotation", &m_localRotation, &debugTransformGetLocalRotation, &debugTransformSetLocalRotation);
	debugAddVec3("Scale", &m_localScale, nullptr, &debugTransformSetLocalScale);
}

void Transform::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator position = dataObject.FindMember("position");
	rapidjson::Value::MemberIterator rotation = dataObject.FindMember("rotation");
	rapidjson::Value::MemberIterator scale = dataObject.FindMember("scale");

	if (position != dataObject.MemberEnd())
	{
		setLocalPosition(XMFLOAT3(position->value["x"].GetFloat(), position->value["y"].GetFloat(), position->value["z"].GetFloat()));
	}

	if (rotation != dataObject.MemberEnd())
	{
		setLocalRotation(XMFLOAT3(rotation->value["x"].GetFloat(), rotation->value["y"].GetFloat(), rotation->value["z"].GetFloat()));
	}

	if (scale != dataObject.MemberEnd())
	{
		setLocalScale(XMFLOAT3(scale->value["x"].GetFloat(), scale->value["y"].GetFloat(), scale->value["z"].GetFloat()));
	}
}

void Transform::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("position");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_localPosition.x);

	writer.Key("y");
	writer.Double(m_localPosition.y);

	writer.Key("z");
	writer.Double(m_localPosition.z);

	writer.EndObject();

	writer.Key("rotation");
	writer.StartObject();

	writer.Key("x");
	writer.Double(XMConvertToDegrees(m_localRotation.x));

	writer.Key("y");
	writer.Double(XMConvertToDegrees(m_localRotation.y));

	writer.Key("z");
	writer.Double(XMConvertToDegrees(m_localRotation.z));

	writer.EndObject();

	writer.Key("scale");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_localScale.x);

	writer.Key("y");
	writer.Double(m_localScale.y);

	writer.Key("z");
	writer.Double(m_localScale.z);

	writer.EndObject();
}

DirectX::XMFLOAT3 Transform::getPosition()
{
	XMMATRIX worldMatrix = XMLoadFloat4x4(&m_worldMatrix);

	if (m_isDirty)
	{
		worldMatrix = calcWorldMatrix();
		m_isDirty = false;
	}

	XMVECTOR worldPositionVec = XMVector3Transform(XMVectorSet(0, 0, 0, 1), worldMatrix);

	XMFLOAT3 worldPosition;
	XMStoreFloat3(&worldPosition, worldPositionVec);

	return worldPosition;
}

XMFLOAT3 Transform::getLocalPosition() const
{
	return m_localPosition;
}

XMFLOAT3 Transform::getLocalRotation() const
{
	return m_localRotation;
}

XMFLOAT3 Transform::getLocalScale() const
{
	return m_localScale;
}

void Transform::setLocalPosition(DirectX::XMFLOAT3 position)
{
	m_localPosition = position;
	setDirty();
}

void Transform::setLocalRotation(DirectX::XMFLOAT3 rotationDegrees)
{
	m_localRotation = XMFLOAT3(XMConvertToRadians(rotationDegrees.x), XMConvertToRadians(rotationDegrees.y), XMConvertToRadians(rotationDegrees.z));
	setDirty();
}

void Transform::setLocalRotationRadians(DirectX::XMFLOAT3 rotationRadians)
{
	m_localRotation = rotationRadians;
	setDirty();
}

void Transform::setLocalScale(DirectX::XMFLOAT3 scale)
{
	m_localScale = scale;
	setDirty();
}

const XMFLOAT3 Transform::getRight()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}

	XMMATRIX rotationMatrix = XMMatrixSet(
		m_worldMatrix.m[0][0] / m_localScale.x, m_worldMatrix.m[0][1], m_worldMatrix.m[0][2], 0,
		m_worldMatrix.m[1][0], m_worldMatrix.m[1][1] / m_localScale.y, m_worldMatrix.m[1][2], 0,
		m_worldMatrix.m[2][0], m_worldMatrix.m[2][1], m_worldMatrix.m[2][2] / m_localScale.z, 0,
		0, 0, 0, 1);

	XMVECTOR v = XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);

	XMFLOAT3 right;
	XMStoreFloat3(&right, v);
	return right;
}

const XMFLOAT3 Transform::getUp()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}

	XMMATRIX rotationMatrix = XMMatrixSet(
		m_worldMatrix.m[0][0] / m_localScale.x, m_worldMatrix.m[0][1], m_worldMatrix.m[0][2], 0,
		m_worldMatrix.m[1][0], m_worldMatrix.m[1][1] / m_localScale.y, m_worldMatrix.m[1][2], 0,
		m_worldMatrix.m[2][0], m_worldMatrix.m[2][1], m_worldMatrix.m[2][2] / m_localScale.z, 0,
		0, 0, 0, 1);

	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

	XMFLOAT3 up;
	XMStoreFloat3(&up, v);
	return up;
}

const XMFLOAT3 Transform::getForward()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}

	XMMATRIX rotationMatrix = XMMatrixSet(
		m_worldMatrix.m[0][0] / m_localScale.x, m_worldMatrix.m[0][1], m_worldMatrix.m[0][2], 0,
		m_worldMatrix.m[1][0], m_worldMatrix.m[1][1] / m_localScale.y, m_worldMatrix.m[1][2], 0,
		m_worldMatrix.m[2][0], m_worldMatrix.m[2][1], m_worldMatrix.m[2][2] / m_localScale.z, 0,
		0, 0, 0, 1);

	XMVECTOR v = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

	XMFLOAT3 forward;
	XMStoreFloat3(&forward, v);
	return forward;
}

const XMFLOAT4X4 Transform::getWorldMatrix()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}
	
	return m_worldMatrix;
}

const XMFLOAT4X4 Transform::getInverseWorldMatrix()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}

	return m_inverseWorldMatrix;
}

void Transform::move(XMFLOAT3 delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->move(delta);
		}
	}
}

void Transform::moveX(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveX(delta);
		}
	}
}

void Transform::moveY(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveY(delta);
		}
	}
}

void Transform::moveZ(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveZ(delta);
		}
	}
}

void Transform::moveLocal(DirectX::XMFLOAT3 delta)
{
	XMFLOAT3 rightFloat3 = getRight();
	XMFLOAT3 upFloat3 = getUp();
	XMFLOAT3 forwardFloat3 = getForward();

	XMVECTOR right = XMLoadFloat3(&rightFloat3);
	XMVECTOR up = XMLoadFloat3(&upFloat3);
	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR positionVec = XMLoadFloat3(&m_localPosition);

	XMVECTOR translationVec = XMVectorAdd(XMVectorAdd(XMVectorScale(right, delta.x), XMVectorScale(up, delta.y)), XMVectorScale(forward, delta.z));
	XMStoreFloat3(&m_localPosition, XMVectorAdd(positionVec, translationVec));

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveLocal(delta);
		}
	}
}

void Transform::moveLocalX(float delta)
{
	XMFLOAT3 rightFloat3 = getRight();

	XMVECTOR right = XMLoadFloat3(&rightFloat3);
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(right, delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveLocalX(delta);
		}
	}
}

void Transform::moveLocalY(float delta)
{
	XMFLOAT3 upFloat3 = getUp();

	XMVECTOR up = XMLoadFloat3(&upFloat3);
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(up, delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveLocalY(delta);
		}
	}
}

void Transform::moveLocalZ(float delta)
{
	XMFLOAT3 forwardFloat3 = getForward();

	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(forward, delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	std::vector<Entity*> children = entity.getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		Transform* childTransform = children[i]->getComponent<Transform>();
		if (childTransform)
		{
			childTransform->moveLocalZ(delta);
		}
	}
}

void Transform::rotateLocal(DirectX::XMFLOAT3 rotDegrees)
{
	XMFLOAT3 rot = XMFLOAT3(XMConvertToRadians(rotDegrees.x), XMConvertToRadians(rotDegrees.y), XMConvertToRadians(rotDegrees.z));
	rotateLocalRadians(rot);
}

void Transform::rotateLocalX(float degrees)
{
	float radians = XMConvertToRadians(degrees);
	rotateLocalXRadians(radians);
}

void Transform::rotateLocalY(float degrees)
{
	float radians = XMConvertToRadians(degrees);
	rotateLocalYRadians(radians);
}

void Transform::rotateLocalZ(float degrees)
{
	float radians = XMConvertToRadians(degrees);
	rotateLocalZRadians(radians);
}

void Transform::rotateLocalRadians(DirectX::XMFLOAT3 rotRadians)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR rotVector = XMLoadFloat3(&rotRadians);
	rotation = XMVectorAdd(rotation, rotVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalXRadians(float radians)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(radians, 0.0f, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalYRadians(float radians)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, radians, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalZRadians(float radians)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, 0.0f, radians, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::scale(DirectX::XMFLOAT3 delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_localScale);
	scale = XMVectorAdd(scale, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_localScale, scale);

	setDirty();
}

void Transform::scaleX(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_localScale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localScale, scale);

	setDirty();
}

void Transform::scaleY(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_localScale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localScale, scale);

	setDirty();
}

void Transform::scaleZ(float delta)
{
	XMVECTOR scale = XMLoadFloat3(&m_localScale);
	scale = XMVectorAdd(scale, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_localScale, scale);

	setDirty();
}

XMMATRIX Transform::calcWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_localRotation));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale));
	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(scale, rotation), translation);

	Entity* parent = entity.getParent();
	if (parent)
	{
		Transform* parentTransform = parent->getComponent<Transform>();
		if (parentTransform)
		{
			world = XMMatrixMultiply(world, parentTransform->calcWorldMatrix());
			XMStoreFloat4x4(&m_worldMatrix, world);
			XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixInverse(nullptr, world));

			return world;
		}
	}

	XMStoreFloat4x4(&m_worldMatrix, world);
	XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixInverse(nullptr, world));

	return world;
}

void Transform::setDirty()
{
	if (!m_isDirty)
	{
		m_isDirty = true;

		std::vector<Entity*> children = entity.getChildren();

		for (unsigned int i = 0; i < children.size(); i++)
		{
			Transform* childTransform = children[i]->getComponent<Transform>();
			if (childTransform)
			{
				childTransform->setDirty();
			}
		}
	}
}

void debugTransformSetLocalPosition(Component* component, const void* value)
{
	XMFLOAT3 position = *static_cast<const XMFLOAT3*>(value);
	static_cast<Transform*>(component)->setLocalPosition(position);
}

void debugTransformGetLocalRotation(const Component* component, void* value)
{
	XMFLOAT3 rotationRadians = static_cast<const Transform*>(component)->getLocalRotation();
	*static_cast<XMFLOAT3*>(value) = XMFLOAT3(XMConvertToDegrees(rotationRadians.x), XMConvertToDegrees(rotationRadians.y), XMConvertToDegrees(rotationRadians.z));
}

void debugTransformSetLocalRotation(Component* component, const void* value)
{
	XMFLOAT3 rotation = *static_cast<const XMFLOAT3*>(value);
	static_cast<Transform*>(component)->setLocalRotation(rotation);
}

void debugTransformSetLocalScale(Component* component, const void* value)
{
	XMFLOAT3 scale = *static_cast<const XMFLOAT3*>(value);
	static_cast<Transform*>(component)->setLocalScale(scale);
}
