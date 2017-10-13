#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform(Entity& entity);
	~Transform();

	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	const DirectX::XMFLOAT3 getPosition() const;
	const DirectX::XMFLOAT3 getRotation() const;
	const DirectX::XMFLOAT3 getScale() const;

	void setPosition(DirectX::XMFLOAT3 position);
	void setRotation(DirectX::XMFLOAT3 rotation);
	void setScale(DirectX::XMFLOAT3 scale);

	const DirectX::XMMATRIX getTranslationMatrix() const;
	const DirectX::XMMATRIX getRotationMatrix() const;
	const DirectX::XMMATRIX getScaleMatrix() const;
	const DirectX::XMMATRIX getWorldMatrix() const;

	const DirectX::XMFLOAT3 getRight() const;
	const DirectX::XMFLOAT3 getUp() const;
	const DirectX::XMFLOAT3 getForward() const;

	void calcTranslationMatrix();
	void calcRotationMatrix();
	void calcScaleMatrix();
	void calcWorldMatrix();

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

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_scale;

	DirectX::XMFLOAT4X4 m_translationMatrix;
	DirectX::XMFLOAT4X4 m_rotationMatrix;
	DirectX::XMFLOAT4X4 m_scaleMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
};