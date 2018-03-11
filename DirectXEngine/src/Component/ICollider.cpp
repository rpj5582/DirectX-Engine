#include "ICollider.h"

#include <memory>

using namespace DirectX;

// Most of the code here was adapted from http://www.dyn4j.org/2010/01/sat/ unless specified otherwise

ICollider::ICollider(Entity& entity, ColliderPolygon* polygons, unsigned int polygonCount) : Component(entity)
{
	m_polygonCount = polygonCount;
	m_polygons = new ColliderPolygon[m_polygonCount];
	memcpy_s(m_polygons, sizeof(ColliderPolygon) * m_polygonCount, polygons, sizeof(ColliderPolygon) * m_polygonCount);
}

ICollider::~ICollider()
{
	delete[] m_polygons;
}

XMFLOAT3 ICollider::doSAT(ICollider& other) const
{
	Transform* transform = entity.getComponent<Transform>();
	Transform* otherTransform = other.entity.getComponent<Transform>();

	if (!transform || !otherTransform) return XMFLOAT3();

	XMFLOAT3 smallestAxis = XMFLOAT3();
	float smallestDisplacement = INFINITY;

	std::vector<XMFLOAT3> axes = calculateFaceNormals(*transform);
	std::vector<XMFLOAT3> otherAxes = other.calculateFaceNormals(*otherTransform);

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

std::vector<XMFLOAT3> ICollider::calculateFaceNormals(Transform& transform) const
{
	// Adapted from https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal (Newell's Method)

	std::vector<XMFLOAT3> normals = std::vector<XMFLOAT3>(m_polygonCount);

	for (unsigned int i = 0; i < normals.size(); i++)
	{
		ColliderPolygon polygon = m_polygons[i];
		XMVECTOR normal = XMVectorSet(0, 0, 0, 0);

		for (unsigned int j = 0; j < polygon.vertexCount; j++)
		{
			// This is a SIMD adaptation of the following code:

			/*normal.x += (currentVertex.y - nextVertex.y) * (currentVertex.z + nextVertex.z);
			normal.y += (currentVertex.z - nextVertex.z) * (currentVertex.x + nextVertex.x);
			normal.z += (currentVertex.x - nextVertex.x) * (currentVertex.y + nextVertex.y);*/

			XMFLOAT3 currentVertex = polygon.vertices[j];
			XMFLOAT3 nextVertex = polygon.vertices[(j + 1) % polygon.vertexCount];

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

		XMFLOAT4X4 worldMatrixFloat4x4 = transform.getWorldMatrix();
		XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

		normal = XMVector3TransformNormal(normal, worldMatrix);
		normal = XMVector3Normalize(normal);

		XMFLOAT3 finalNormal;
		XMStoreFloat3(&finalNormal, normal);

		normals[i] = finalNormal;
	}

	return normals;
}

std::pair<float, float> ICollider::project(Transform& transform, XMFLOAT3 axis) const
{
	XMFLOAT4X4 worldMatrixFloat4x4 = transform.getWorldMatrix();
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

	XMVECTOR axisVec = XMLoadFloat3(&axis);

	float min = INFINITY;
	float max = -INFINITY;

	for (unsigned int i = 0; i < m_polygonCount; i++)
	{
		ColliderPolygon polygon = m_polygons[i];
		for (unsigned int j = 0; j < polygon.vertexCount; j++)
		{
			XMVECTOR vertex = XMLoadFloat3(&polygon.vertices[j]);
			XMVECTOR dotResult = XMVector3Dot(XMVector3TransformCoord(vertex, worldMatrix), axisVec);

			float projection;
			XMStoreFloat(&projection, dotResult);

			if (projection < min)
				min = projection;
			else if (projection > max)
				max = projection;
		}
	}
	
	return std::pair<float, float>(min, max);
}
