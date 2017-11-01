#pragma once
#include "Component.h"

class GUITransform : public Component
{
public:
	GUITransform(Entity& entity);
	~GUITransform();

	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	bool containsPoint(float x, float y) const;

	DirectX::XMFLOAT2 getPosition() const;
	void setPosition(DirectX::XMFLOAT2 position);
	void move(DirectX::XMFLOAT2 delta);
	void moveX(float deltaX);
	void moveY(float deltaY);

	float getRotation() const;
	void setRotation(float degrees);
	void rotate(float degrees);

	DirectX::XMFLOAT2 getSize() const;
	void setSize(DirectX::XMFLOAT2 size);

	DirectX::XMFLOAT2 getOrigin() const;
	void setOrigin(DirectX::XMFLOAT2 origin);

private:
	DirectX::XMFLOAT2 m_position;
	float m_rotation;
	DirectX::XMFLOAT2 m_size;
	DirectX::XMFLOAT2 m_origin;
};