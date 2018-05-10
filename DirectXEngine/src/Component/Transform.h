#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform(Entity& entity);
	~Transform();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	DirectX::XMFLOAT3 getPosition();
	DirectX::XMFLOAT3 getLocalPosition() const;
	DirectX::XMFLOAT3 getLocalRotation() const;
	DirectX::XMFLOAT3 getLocalScale() const;

	void setLocalPosition(DirectX::XMFLOAT3 position);
	void setLocalRotation(DirectX::XMFLOAT3 rotationDegrees);
	void setLocalRotationRadians(DirectX::XMFLOAT3 rotationRadians);
	void setLocalScale(DirectX::XMFLOAT3 scale);

	const DirectX::XMFLOAT3 getRight();
	const DirectX::XMFLOAT3 getUp();
	const DirectX::XMFLOAT3 getForward();

	const DirectX::XMFLOAT4X4 getWorldMatrix();
	const DirectX::XMFLOAT4X4 getInverseWorldMatrix();

	void move(DirectX::XMFLOAT3 delta);
	void moveX(float delta);
	void moveY(float delta);
	void moveZ(float delta);

	void moveLocal(DirectX::XMFLOAT3 delta);
	void moveLocalX(float delta);
	void moveLocalY(float delta);
	void moveLocalZ(float delta);

	void rotateLocal(DirectX::XMFLOAT3 rotDegrees);
	void rotateLocalX(float degrees);
	void rotateLocalY(float degrees);
	void rotateLocalZ(float degrees);

	void rotateLocalRadians(DirectX::XMFLOAT3 rotRadians);
	void rotateLocalXRadians(float radians);
	void rotateLocalYRadians(float radians);
	void rotateLocalZRadians(float radians);

	void scale(DirectX::XMFLOAT3 delta);
	void scaleX(float delta);
	void scaleY(float delta);
	void scaleZ(float delta);

	void setDirty();

private:
	DirectX::XMMATRIX calcWorldMatrix();

	DirectX::XMFLOAT3 m_localPosition;
	DirectX::XMFLOAT3 m_localRotation;
	DirectX::XMFLOAT3 m_localScale;

	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_inverseWorldMatrix;
	bool m_isDirty;
};

void debugTransformSetLocalPosition(Component* component, const void* value);
void debugTransformGetLocalRotation(const Component* component, void* value);
void debugTransformSetLocalRotation(Component* component, const void* value);
void debugTransformSetLocalScale(Component* component, const void* value);