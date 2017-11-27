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

	const DirectX::XMFLOAT3 getPosition();
	const DirectX::XMFLOAT3 getLocalPosition() const;
	const DirectX::XMFLOAT3 getLocalRotation() const;
	const DirectX::XMFLOAT3 getLocalScale() const;

	void setLocalPosition(DirectX::XMFLOAT3 position);
	void setLocalRotation(DirectX::XMFLOAT3 rotation);
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

void TW_CALL getTransformPositionDebugEditor(void* value, void* clientData);
void TW_CALL getTransformRotationDebugEditor(void* value, void* clientData);
void TW_CALL getTransformScaleDebugEditor(void* value, void* clientData);

void TW_CALL setTransformPositionDebugEditor(const void* value, void* clientData);
void TW_CALL setTransformRotationDebugEditor(const void* value, void* clientData);
void TW_CALL setTransformScaleDebugEditor(const void* value, void* clientData);