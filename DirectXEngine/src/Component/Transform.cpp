#include "Transform.h"

#include "ComponentRegistry.h"
#include "../Scene/Scene.h"

using namespace DirectX;

Transform::Transform(Entity& entity) : Component(entity)
{
	m_localPosition = XMFLOAT3();
	m_localRotation = XMFLOAT3();
	m_localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixIdentity());
	m_isDirty = false;

	m_parent = nullptr;
	m_children = std::vector<Transform*>();
}

Transform::~Transform()
{
}

void Transform::init()
{
	Component::init();

	Debug::entityDebugWindow->addVariableWithCallbacks(Debug::TW_TYPE_VEC3F, "Position", typeName, entity.getName(), &getTransformPositionDebugEditor, &setTransformPositionDebugEditor, this);
	Debug::entityDebugWindow->addVariableWithCallbacks(Debug::TW_TYPE_VEC3F, "Rotation", typeName, entity.getName(), &getTransformRotationDebugEditor, &setTransformRotationDebugEditor, this);
	Debug::entityDebugWindow->addVariableWithCallbacks(Debug::TW_TYPE_VEC3F, "Scale", typeName, entity.getName(), &getTransformScaleDebugEditor, &setTransformScaleDebugEditor, this);

	/*Debug::entityDebugWindow->addVariable(&d_parentNameInputField, TW_TYPE_STDSTRING, "Parent Name", typeName, entity.getName());
	Debug::entityDebugWindow->addButton(entity.getName() + typeName + "SetParentRemoveButton", "Set Parent", entity.getName() + typeName, &setTransformParentNameDebugEditor, this);
	Debug::entityDebugWindow->addSeparator(entity.getName() + typeName + "SetParentSeparator", entity.getName() + typeName);

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		Debug::entityDebugWindow->addVariable(&d_childNameInputField, TW_TYPE_STDSTRING, "Child Name", typeName, entity.getName());
	}*/
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

void Transform::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
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

const DirectX::XMFLOAT3 Transform::getPosition()
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

const XMFLOAT3 Transform::getLocalPosition() const
{
	return m_localPosition;
}

const XMFLOAT3 Transform::getLocalRotation() const
{
	return m_localRotation;
}

const XMFLOAT3 Transform::getLocalScale() const
{
	return m_localScale;
}

void Transform::setLocalPosition(DirectX::XMFLOAT3 position)
{
	m_localPosition = position;
	setDirty();
}

void Transform::setLocalRotation(DirectX::XMFLOAT3 rotation)
{
	m_localRotation = XMFLOAT3(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
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

const XMMATRIX Transform::getWorldMatrix()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}
	
	return XMLoadFloat4x4(&m_worldMatrix);
}

const DirectX::XMMATRIX Transform::getInverseWorldMatrix()
{
	if (m_isDirty)
	{
		calcWorldMatrix();
		m_isDirty = false;
	}

	return XMLoadFloat4x4(&m_inverseWorldMatrix);
}

void Transform::move(XMFLOAT3 delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMLoadFloat3(&delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->move(delta);
	}
}

void Transform::moveX(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveX(delta);
	}
}

void Transform::moveY(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveY(delta);
	}
}

void Transform::moveZ(float delta)
{
	XMVECTOR position = XMLoadFloat3(&m_localPosition);
	position = XMVectorAdd(position, XMVectorScale(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), delta));
	XMStoreFloat3(&m_localPosition, position);

	setDirty();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveZ(delta);
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

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveLocal(delta);
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

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveLocalX(delta);
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

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveLocalY(delta);
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

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->moveLocalZ(delta);
	}
}

void Transform::rotateLocal(DirectX::XMFLOAT3 rotDegrees)
{
	XMFLOAT3 rot = XMFLOAT3(XMConvertToRadians(rotDegrees.x), XMConvertToRadians(rotDegrees.y), XMConvertToRadians(rotDegrees.z));
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR rotVector = XMLoadFloat3(&rot);
	rotation = XMVectorAdd(rotation, rotVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalX(float degrees)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(XMConvertToRadians(degrees), 0.0f, 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalY(float degrees)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, XMConvertToRadians(degrees), 0.0f, 0.0f);
	rotation = XMVectorAdd(rotation, angleVector);
	XMStoreFloat3(&m_localRotation, rotation);

	setDirty();
}

void Transform::rotateLocalZ(float degrees)
{
	XMVECTOR rotation = XMLoadFloat3(&m_localRotation);
	XMVECTOR angleVector = XMVectorSet(0.0f, 0.0f, XMConvertToRadians(degrees), 0.0f);
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

Transform* Transform::getParent() const
{
	return m_parent;
}

void Transform::setParent(Transform* parent)
{
	if (this == parent)
	{
		Debug::warning("Parent not assigned to " + entity.getName() + " because the parent given was itself.");
		return;
	}

	if (parent)
	{
		setParentNonRecursive(parent);
		parent->addChildNonRecursive(this);
	}
	else
	{
		m_parent->removeChild(this);
	}		
}

Transform* Transform::getChild(unsigned int index) const
{
	if (index >= 0 && index < m_children.size())
		return m_children.at(index);

	Debug::warning("Child index " + std::to_string(index) + " outside bounds of children list for entity " + entity.getName());
	return nullptr;
}

Transform* Transform::getChildByName(std::string childName) const
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->getName() == childName)
			return m_children[i];
	}

	Debug::warning("Child with name " + childName + " could not be found in child list for entity " + entity.getName());
	return nullptr;
}

std::vector<Transform*> Transform::getChildren() const
{
	return m_children;
}

void Transform::addChild(Transform* child)
{
	if (!child)
	{
		Debug::warning("Attempted to add null child to entity " + entity.getName());
		return;
	}

	if (this == child)
	{
		Debug::warning("Child not added to " + entity.getName() + " because the child given was itself.");
		return;
	}

	addChildNonRecursive(child);
	child->setParentNonRecursive(this);
}

void Transform::removeChild(Transform* child)
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] == child)
		{
			m_children[i]->setParentNonRecursive(nullptr);
			m_children.erase(m_children.begin() + i);
			return;
		}
	}

	Debug::warning("Child with name " + entity.getName() + " could not be found in the child list of entity " + entity.getName());
}

void Transform::removeChildByIndex(unsigned int index)
{
	if (index >= 0 && index < m_children.size())
	{
		m_children.at(index)->setParentNonRecursive(nullptr);
		m_children.erase(m_children.begin() + index);
		return;
	}

	Debug::warning("Child index " + std::to_string(index) + " outside bounds of children list for entity " + entity.getName());
}

void Transform::removeChildByName(std::string childName)
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->getName() == childName)
		{
			m_children.at(i)->setParentNonRecursive(nullptr);
			m_children.erase(m_children.begin() + i);
			return;
		}
	}

	Debug::warning("Child with name " + childName + " could not be found in child list for entity " + entity.getName());
}

void Transform::removeAllChildren()
{
	m_children.clear();
}

XMMATRIX Transform::calcWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_localPosition));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_localRotation));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_localScale));
	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(scale, rotation), translation);

	if (m_parent)
	{
		world = XMMatrixMultiply(world, m_parent->calcWorldMatrix());
		XMStoreFloat4x4(&m_worldMatrix, world);
		XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixInverse(nullptr, world));
	}
	else
	{
		XMStoreFloat4x4(&m_worldMatrix, world);
		XMStoreFloat4x4(&m_inverseWorldMatrix, XMMatrixInverse(nullptr, world));
	}

	return world;
}

void Transform::setDirty()
{
	if (!m_isDirty)
	{
		m_isDirty = true;

		for (unsigned int i = 0; i < m_children.size(); i++)
		{
			m_children[i]->setDirty();
		}
	}
}

void Transform::setParentNonRecursive(Transform* parent)
{
	m_parent = parent;
}

void Transform::addChildNonRecursive(Transform* child)
{
	m_children.push_back(child);
}

void TW_CALL getTransformPositionDebugEditor(void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	*static_cast<XMFLOAT3*>(value) = transform->getLocalPosition();
}

void TW_CALL getTransformRotationDebugEditor(void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	XMFLOAT3 rotation = transform->getLocalRotation();
	XMFLOAT3 rotationDegrees = XMFLOAT3(XMConvertToDegrees(rotation.x), XMConvertToDegrees(rotation.y), XMConvertToDegrees(rotation.z));
	*static_cast<XMFLOAT3*>(value) = rotationDegrees;
}

void TW_CALL getTransformScaleDebugEditor(void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	*static_cast<XMFLOAT3*>(value) = transform->getLocalScale();
}

void TW_CALL setTransformPositionDebugEditor(const void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	transform->setLocalPosition(*static_cast<const XMFLOAT3*>(value));
}

void TW_CALL setTransformRotationDebugEditor(const void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	transform->setLocalRotation(*static_cast<const XMFLOAT3*>(value));
}

void TW_CALL setTransformScaleDebugEditor(const void* value, void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	transform->setLocalScale(*static_cast<const XMFLOAT3*>(value));
}

void TW_CALL setTransformParentNameDebugEditor(void* clientData)
{
	Transform* transform = static_cast<Transform*>(clientData);
	transform->setParent(transform->getEntity().getScene().getEntityByName(transform->d_parentNameInputField)->getComponent<Transform>());
}
