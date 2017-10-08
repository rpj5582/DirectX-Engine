#pragma once
#include "Component.h"

class GUITransform : public Component
{
public:
	GUITransform(Scene* scene, unsigned int entity);
	~GUITransform();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

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


private:
	DirectX::XMFLOAT2 m_position;
	float m_rotation;
	DirectX::XMFLOAT2 m_size;
};