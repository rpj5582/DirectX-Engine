#pragma once
#include "Component.h"

#include "Transform.h"

#include <DirectXMath.h>
#include <vector>

class ICollider : public Component
{
public:
	ICollider(Entity& entity, Mesh* collisionMesh);
	~ICollider();

	virtual void init() override;
	virtual void initDebugVariables() override;
	void loadFromJSON(rapidjson::Value& dataObject) override;
	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	DirectX::XMFLOAT3 doSAT(ICollider& other) const;

	const Mesh* const getMesh() const;

	DirectX::XMFLOAT3 getOffset() const;
	DirectX::XMFLOAT3 getScale() const;
	DirectX::XMFLOAT4X4 getOffsetScaleMatrix() const;

	void setOffset(DirectX::XMFLOAT3 offset);
	void setScale(DirectX::XMFLOAT3 scale);

private:
	std::vector<DirectX::XMFLOAT3> calculateFaceNormals() const;
	std::pair<float, float> project(Transform& transform, DirectX::XMFLOAT3 axis) const;
	void updateOffsetScaleMatrix();

	Mesh* m_collisionMesh;
	std::vector<DirectX::XMFLOAT3> m_faceNormals;

	DirectX::XMFLOAT3 m_offset;
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT4X4 m_offsetScaleMatrix;
};

void debugColliderGetOffset(const Component* component, void* value);
void debugColliderSetOffset(Component* component, const void* value);
void debugColliderGetScale(const Component* component, void* value);
void debugColliderSetScale(Component* component, const void* value);