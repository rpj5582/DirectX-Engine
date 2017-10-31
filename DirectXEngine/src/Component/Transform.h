#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform(Entity& entity);
	~Transform();

	virtual void init() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

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

	const DirectX::XMMATRIX getWorldMatrix();
	const DirectX::XMMATRIX getInverseWorldMatrix();

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

	Transform* getParent() const;
	void setParent(Transform* parent);

	Transform* getChild(unsigned int index) const;
	Transform* getChildByName(std::string childName) const;
	std::vector<Transform*> getChildren() const;

	void addChild(Transform* child);
	void removeChild(Transform* child);
	void removeChildByIndex(unsigned int index);
	void removeChildByName(std::string childName);
	void removeAllChildren();

	std::string d_parentNameInputField;
	std::string d_childNameInputField;
	std::vector<std::string> d_childrenNames;

private:
	DirectX::XMMATRIX calcWorldMatrix();
	void setDirty();

	void setParentNonRecursive(Transform* parent);
	void addChildNonRecursive(Transform* child);

	DirectX::XMFLOAT3 m_localPosition;
	DirectX::XMFLOAT3 m_localRotation;
	DirectX::XMFLOAT3 m_localScale;

	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_inverseWorldMatrix;
	bool m_isDirty;

	Transform* m_parent;
	std::vector<Transform*> m_children;
};

void TW_CALL getTransformPositionDebugEditor(void* value, void* clientData);
void TW_CALL getTransformRotationDebugEditor(void* value, void* clientData);
void TW_CALL getTransformScaleDebugEditor(void* value, void* clientData);

void TW_CALL setTransformPositionDebugEditor(const void* value, void* clientData);
void TW_CALL setTransformRotationDebugEditor(const void* value, void* clientData);
void TW_CALL setTransformScaleDebugEditor(const void* value, void* clientData);

void TW_CALL setTransformParentNameDebugEditor(void* clientData);