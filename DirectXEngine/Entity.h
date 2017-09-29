#pragma once

#include "Component.h"
#include "Mesh.h"
#include "Material.h"

class Entity
{
public:
	Entity(Mesh* mesh, Material* material);
	~Entity();

	template<typename T>
	T* addComponent();

	template<typename T>
	T* getComponent() const;

	template<typename T>
	bool removeComponent();

	Mesh* getMesh() const;
	Material* getMaterial() const;

	const DirectX::XMFLOAT3 getPosition() const;
	const DirectX::XMFLOAT3 getRotation() const;
	const DirectX::XMFLOAT3 getScale() const;

	const DirectX::XMMATRIX getTranslationMatrix() const;
	const DirectX::XMMATRIX getRotationMatrix() const;
	const DirectX::XMMATRIX getScaleMatrix() const;
	const DirectX::XMMATRIX getWorldMatrix() const;
	const DirectX::XMMATRIX getWorldMatrixInverseTranspose() const;

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

	void rotateLocal(DirectX::XMFLOAT3 rot);
	void rotateLocalX(float angle);
	void rotateLocalY(float angle);
	void rotateLocalZ(float angle);

	void scale(DirectX::XMFLOAT3 delta);
	void scaleX(float delta);
	void scaleY(float delta);
	void scaleZ(float delta);

private:
	std::vector<Component*> m_components;

	Mesh* m_mesh;
	Material* m_material;

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_scale;

	DirectX::XMFLOAT4X4 m_translationMatrix;
	DirectX::XMFLOAT4X4 m_rotationMatrix;
	DirectX::XMFLOAT4X4 m_scaleMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrixInverseTranspose;
};

template<typename T>
inline T* Entity::addComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");
	
	// Don't allow more than one of the same type of component
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (typeid(m_components[i]) == typeid(T))
			return nullptr;
	}

	T* component = new T(this);
	m_components.push_back(component);
	return component;
}

template<typename T>
inline T* Entity::getComponent() const
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (typeid(m_components[i]) == typeid(T))
			return m_components[i];
	}

	return nullptr;
}

template<typename T>
inline bool Entity::removeComponent()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (typeid(m_components[i]) == typeid(T))
		{
			delete m_components[i];
			m_components.erase(m_components.begin() + i);
			return true;
		}
	}

	return false;
}
