#include "ICollider.h"

#include <memory>

using namespace DirectX;

// Most of the code here was adapted from http://www.dyn4j.org/2010/01/sat/ unless specified otherwise

ICollider::ICollider(Entity& entity, Mesh* collisionMesh) : Component(entity)
{
	m_collisionMesh = collisionMesh;

	// Precompute the face normals, as each individual triangle needs to be evaluated
	m_faceNormals = calculateFaceNormals();

	m_offset = XMFLOAT3();
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	updateOffsetScaleMatrix();
}

ICollider::~ICollider()
{
	if (!entity.getComponent<ICollider>())
	{
		if (entity.hasTag(TAG_COLLIDER))
			entity.removeTag(TAG_COLLIDER);
	}
}

void ICollider::init()
{
	Component::init();

	if (!entity.hasTag(TAG_COLLIDER))
		entity.addTag(TAG_COLLIDER);
}

void ICollider::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddVec3("Offset", &m_offset, &debugColliderGetOffset, &debugColliderSetOffset);
	debugAddVec3("Scale", &m_scale, &debugColliderGetScale, &debugColliderSetScale);
}

void ICollider::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator offset = dataObject.FindMember("offset");
	if (offset != dataObject.MemberEnd())
	{
		setOffset(XMFLOAT3(offset->value["x"].GetFloat(), offset->value["y"].GetFloat(), offset->value["z"].GetFloat()));
	}

	rapidjson::Value::MemberIterator scale = dataObject.FindMember("scale");
	if (scale != dataObject.MemberEnd())
	{
		setScale(XMFLOAT3(scale->value["x"].GetFloat(), scale->value["y"].GetFloat(), scale->value["z"].GetFloat()));
	}
}

void ICollider::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("offset");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_offset.x);

	writer.Key("y");
	writer.Double(m_offset.y);

	writer.Key("z");
	writer.Double(m_offset.z);

	writer.EndObject();

	writer.Key("scale");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_scale.x);

	writer.Key("y");
	writer.Double(m_scale.y);

	writer.Key("z");
	writer.Double(m_scale.z);

	writer.EndObject();
}

XMFLOAT3 ICollider::doSAT(ICollider& other) const
{
	Transform* transform = entity.getComponent<Transform>();
	Transform* otherTransform = other.entity.getComponent<Transform>();

	if (!transform || !otherTransform) return XMFLOAT3();

	XMFLOAT3 smallestAxis = XMFLOAT3();
	float smallestDisplacement = INFINITY;

	XMFLOAT4X4 worldMatrixFloat4x4 = transform->getWorldMatrix();
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

	XMFLOAT4X4 otherWorldMatrixFloat4x4 = otherTransform->getWorldMatrix();
	XMMATRIX otherWorldMatrix = XMLoadFloat4x4(&otherWorldMatrixFloat4x4);

	std::vector<XMFLOAT3> axes = std::vector<XMFLOAT3>(m_faceNormals.size());
	for (unsigned int i = 0; i < m_faceNormals.size(); i++)
	{
		XMVECTOR faceNormal = XMLoadFloat3(&m_faceNormals[i]);
		XMVECTOR transformedFaceNormal = XMVector3Normalize(XMVector3TransformNormal(faceNormal, worldMatrix));
		XMStoreFloat3(&axes[i], transformedFaceNormal);
	}

	std::vector<XMFLOAT3> otherAxes = std::vector<XMFLOAT3>(other.m_faceNormals.size());
	for (unsigned int i = 0; i < other.m_faceNormals.size(); i++)
	{
		XMVECTOR faceNormal = XMLoadFloat3(&other.m_faceNormals[i]);
		XMVECTOR transformedFaceNormal = XMVector3Normalize(XMVector3TransformNormal(faceNormal, otherWorldMatrix));
		XMStoreFloat3(&otherAxes[i], transformedFaceNormal);
	}

	// Check all axes from first collider
	for (unsigned int i = 0; i < axes.size(); i++)
	{
		XMFLOAT3 axis = axes[i];

		std::pair<float, float> projection = project(*transform, axis);
		std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

		// If the projections don't intersect, the colliders don't intersect
		if (otherProjection.first > projection.second || projection.first > otherProjection.second) return XMFLOAT3();
		else
		{
			float displacement;

			float midpoint = (projection.second + projection.first) / 2.0f;
			if (abs(midpoint - projection.second) > abs(midpoint - projection.first))
				displacement = projection.second - otherProjection.first;
			else
				displacement = otherProjection.second - projection.first;

			if (abs(displacement) < abs(smallestDisplacement))
			{
				smallestDisplacement = displacement;
				smallestAxis = axis;
			}
		}
	}

	// Check all axes from second collider
	for (unsigned int i = 0; i < otherAxes.size(); i++)
	{
		XMFLOAT3 axis = otherAxes[i];

		std::pair<float, float> projection = project(*transform, axis);
		std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

		// If the projections don't intersect, the colliders don't intersect
		if (otherProjection.first > projection.second || projection.first > otherProjection.second) return XMFLOAT3();
		else
		{
			float displacement;

			float midpoint = (projection.second + projection.first) / 2.0f;
			if (abs(midpoint - projection.second) > abs(midpoint - projection.first))
				displacement = projection.second - otherProjection.first;
			else
				displacement = otherProjection.second - projection.first;

			if (abs(displacement) < abs(smallestDisplacement))
			{
				smallestDisplacement = displacement;
				smallestAxis = axis;
			}
		}
	}

	// Check all axes generated from all pairs of axis cross products
	for (unsigned int i = 0; i < axes.size(); i++)
	{
		XMVECTOR axis1 = XMLoadFloat3(&axes[i]);
		for (unsigned int j = 0; j < otherAxes.size(); j++)
		{
			XMVECTOR axis2 = XMLoadFloat3(&otherAxes[j]);

			XMVECTOR cross = XMVector3Cross(axis1, axis2);
			if (XMVectorEqual(cross, XMVectorSet(0, 0, 0, 0)).m128_f32[0]) continue;

			XMFLOAT3 axis;
			XMStoreFloat3(&axis, cross);

			std::pair<float, float> projection = project(*transform, axis);
			std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

			// If the projections don't intersect, the colliders don't intersect
			if (otherProjection.first > projection.second || projection.first > otherProjection.second) return XMFLOAT3();
			else
			{
				float displacement;

				float midpoint = (projection.second + projection.first) / 2.0f;
				if (abs(midpoint - projection.second) > abs(midpoint - projection.first))
					displacement = projection.second - otherProjection.first;
				else
					displacement = otherProjection.second - projection.first;

				if (abs(displacement) < abs(smallestDisplacement))
				{
					smallestDisplacement = displacement;
					smallestAxis = axis;
				}
			}
		}
	}

	XMVECTOR mtvVec = XMVectorScale(XMLoadFloat3(&smallestAxis), smallestDisplacement);
	XMFLOAT3 mtv;
	XMStoreFloat3(&mtv, mtvVec);

	return mtv;
}

const Mesh* const ICollider::getMesh() const
{
	return m_collisionMesh;
}

DirectX::XMFLOAT3 ICollider::getOffset() const
{
	return m_offset;
}

DirectX::XMFLOAT3 ICollider::getScale() const
{
	return m_scale;
}

DirectX::XMFLOAT4X4 ICollider::getOffsetScaleMatrix() const
{
	return m_offsetScaleMatrix;
}

void ICollider::setOffset(DirectX::XMFLOAT3 offset)
{
	m_offset = offset;
	updateOffsetScaleMatrix();
}

void ICollider::setScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	updateOffsetScaleMatrix();
}

std::vector<XMFLOAT3> ICollider::calculateFaceNormals() const
{
	// Adapted from https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal (Newell's Method)

	std::vector<XMFLOAT3> normals = std::vector<XMFLOAT3>();

	const Vertex* vertices = m_collisionMesh->getVertices();
	const unsigned int* indices = m_collisionMesh->getIndices();
	unsigned int indexCount = m_collisionMesh->getIndexCount();

	for (unsigned int i = 0; i < indexCount;)
	{
		XMVECTOR normal = XMVectorSet(0, 0, 0, 0);

		for (unsigned int j = 0; j < 3; j++)
		{
			unsigned int currentIndex = indices[i + j];
			unsigned int nextIndex = indices[i + ((j + 1) % 3)];

			// This is a SIMD adaptation of the following code:

			/*normal.x += (currentVertex.y - nextVertex.y) * (currentVertex.z + nextVertex.z);
			normal.y += (currentVertex.z - nextVertex.z) * (currentVertex.x + nextVertex.x);
			normal.z += (currentVertex.x - nextVertex.x) * (currentVertex.y + nextVertex.y);*/

			XMFLOAT3 currentVertex = vertices[currentIndex].position;
			XMFLOAT3 nextVertex = vertices[nextIndex].position;

			XMVECTOR current = XMLoadFloat3(&currentVertex);
			XMVECTOR next = XMLoadFloat3(&nextVertex);

			XMVECTOR sub = XMVectorSubtract(current, next);
			XMVECTOR sum = XMVectorAdd(current, next);

			XMFLOAT3 subFloat3;
			XMFLOAT3 sumFloat3;
			XMStoreFloat3(&subFloat3, sub);
			XMStoreFloat3(&sumFloat3, sum);

			XMFLOAT3 modfiedSub = XMFLOAT3(subFloat3.y, subFloat3.z, subFloat3.x);
			XMFLOAT3 modfiedSum = XMFLOAT3(sumFloat3.z, sumFloat3.x, sumFloat3.y);

			sub = XMLoadFloat3(&modfiedSub);
			sum = XMLoadFloat3(&modfiedSum);

			normal = XMVectorMultiplyAdd(sub, sum, normal);
		}

		XMFLOAT3 finalNormal;
		XMStoreFloat3(&finalNormal, XMVector3Normalize(normal));

		// Check to see if we already have this normal or the opposite normal, if we do, we don't need to keep it
		bool keepNormal = true;
		for (unsigned int j = 0; j < normals.size(); j++)
		{
			XMFLOAT3 oldNormal = normals[j];

			if (oldNormal.x == finalNormal.x && oldNormal.y == finalNormal.y && oldNormal.z == finalNormal.z || 
				oldNormal.x == -finalNormal.x && oldNormal.y == -finalNormal.y && oldNormal.z == -finalNormal.z)
			{
				keepNormal = false;
				break;
			}
		}

		if(keepNormal)
			normals.push_back(finalNormal);

		i += 3;
	}

	return normals;
}

std::pair<float, float> ICollider::project(Transform& transform, XMFLOAT3 axis) const
{
	XMFLOAT4X4 worldMatrixFloat4x4 = transform.getWorldMatrix();
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

	XMMATRIX offsetScaleMatrix = XMLoadFloat4x4(&m_offsetScaleMatrix);

	XMVECTOR axisVec = XMLoadFloat3(&axis);

	float min = INFINITY;
	float max = -INFINITY;

	const Vertex* vertices = m_collisionMesh->getVertices();
	unsigned int vertexCount = m_collisionMesh->getVertexCount();

	for (unsigned int i = 0; i < vertexCount; i++)
	{
		XMVECTOR vertex = XMLoadFloat3(&vertices[i].position);
		XMVECTOR vertexOffsetScaled = XMVector3TransformCoord(vertex, offsetScaleMatrix);
		XMVECTOR dotResult = XMVector3Dot(XMVector3TransformCoord(vertexOffsetScaled, worldMatrix), axisVec);

		float projection;
		XMStoreFloat(&projection, dotResult);

		if (projection < min)
			min = projection;
		else if (projection > max)
			max = projection;
	}
	
	return std::pair<float, float>(min, max);
}

void ICollider::updateOffsetScaleMatrix()
{
	XMVECTOR offset = XMLoadFloat3(&m_offset);
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
	XMMATRIX offsetMatrix = XMMatrixTranslationFromVector(offset);
	
	XMMATRIX offsetScaleMatrix = XMMatrixMultiply(scaleMatrix, offsetMatrix);

	XMStoreFloat4x4(&m_offsetScaleMatrix, offsetScaleMatrix);
}

void debugColliderGetOffset(const Component* component, void* value)
{
	XMFLOAT3 offset = static_cast<const ICollider*>(component)->getOffset();
	*static_cast<XMFLOAT3*>(value) = offset;
}

void debugColliderSetOffset(Component* component, const void* value)
{
	const XMFLOAT3 offset = *static_cast<const XMFLOAT3*>(value);
	static_cast<ICollider*>(component)->setOffset(offset);
}

void debugColliderGetScale(const Component* component, void* value)
{
	XMFLOAT3 scale = static_cast<const ICollider*>(component)->getScale();
	*static_cast<XMFLOAT3*>(value) = scale;
}

void debugColliderSetScale(Component* component, const void* value)
{
	const XMFLOAT3 scale = *static_cast<const XMFLOAT3*>(value);
	static_cast<ICollider*>(component)->setScale(scale);
}
