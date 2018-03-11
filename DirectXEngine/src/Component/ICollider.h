#pragma once
#include "Component.h"

#include "Transform.h"

#include <DirectXMath.h>
#include <vector>

struct ColliderPolygon
{
	DirectX::XMFLOAT3* vertices;
	unsigned int vertexCount;
};

class ICollider : public Component
{
public:
	ICollider(Entity& entity, ColliderPolygon* polygons, unsigned int polygonCount);
	~ICollider();

	DirectX::XMFLOAT3 doSAT(ICollider& other) const;

private:
	std::vector<DirectX::XMFLOAT3> calculateFaceNormals(Transform& transform) const;
	std::pair<float, float> project(Transform& transform, DirectX::XMFLOAT3 axis) const;

	ColliderPolygon* m_polygons;
	unsigned int m_polygonCount;
};