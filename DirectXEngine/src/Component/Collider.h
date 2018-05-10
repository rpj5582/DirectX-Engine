#pragma once
#include "Component.h"

#include "Transform.h"

#include <DirectXMath.h>
#include <vector>

class Collider : public Component
{
public:
	Collider(Entity& entity);
	~Collider();

	void init() override;
	void initDebugVariables() override;
	void loadFromJSON(rapidjson::Value& dataObject) override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	bool calculateMTV(Collider& other, DirectX::XMFLOAT3& mtv) const;
	DirectX::XMFLOAT3 calculateContactPoint(Collider& other, DirectX::XMFLOAT3 mtvNormal);

	Mesh* const getMesh() const;
	void setMesh(Mesh* const mesh);

	DirectX::XMFLOAT3 getOffset() const;
	DirectX::XMFLOAT3 getScale() const;
	DirectX::XMFLOAT4X4 getOffsetScaleMatrix() const;

	void setOffset(DirectX::XMFLOAT3 offset);
	void setScale(DirectX::XMFLOAT3 scale);

private:
	std::vector<DirectX::XMFLOAT3> calculateFaceNormals() const;
	std::pair<float, float> project(Transform& transform, DirectX::XMFLOAT3 axis) const;
	float overlap(std::pair<float, float> projection, std::pair<float, float> otherProjection) const;
	
	DirectX::XMFLOAT3 faceToPoint(const DirectX::XMFLOAT4X4& worldMatrixFloat4x4, const DirectX::XMFLOAT3& mtvNormal, DirectX::XMFLOAT3* closestPoints, int& totalSuspects, float startingInf);
	DirectX::XMFLOAT3 edgeToEdge(const DirectX::XMFLOAT4X4& worldMatrixFloat4x4, const DirectX::XMFLOAT4X4& otherWorldMatrixFloat4x4, DirectX::XMFLOAT3* closestPoints, DirectX::XMFLOAT3* otherClosestPoints);
	DirectX::XMFLOAT3 calculateContactPoint1D(Transform& transform, Transform& otherTransform, const DirectX::XMFLOAT3 axis);

	void updateOffsetScaleMatrix();

	Mesh* m_collisionMesh;
	std::vector<DirectX::XMFLOAT3> m_faceNormals;

	DirectX::XMFLOAT3 m_offset;
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT4X4 m_offsetScaleMatrix;
};

void debugColliderGetMesh(const Component* component, void* value);
void debugColliderSetMesh(Component* component, const void* value);
void debugColliderGetOffset(const Component* component, void* value);
void debugColliderSetOffset(Component* component, const void* value);
void debugColliderGetScale(const Component* component, void* value);
void debugColliderSetScale(Component* component, const void* value);