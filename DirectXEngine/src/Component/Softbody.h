#pragma once
#include "IPhysicsBody.h"

class Softbody : public IPhysicsBody
{
public:
	Softbody(Entity& entity);
	~Softbody();

	void init() override;
	void initDebugVariables() override;
	void update(float deltaTime, float totalTime) override;

	void integrateForces() override;
	void integrateVelocity() override;

	BodyData* getClosestBodyData(DirectX::XMFLOAT3 point, unsigned int* i = nullptr, unsigned int* j = nullptr, unsigned int* k = nullptr) override;

	void updateVisual() override;

	void applyForce(DirectX::XMFLOAT3 force, BodyData* body);
	void applyTorque(DirectX::XMFLOAT3 force, BodyData* body);

private:
	DirectX::XMFLOAT3 calculateCenterOfMass();

	DirectX::XMFLOAT3 m_size;
	unsigned int m_massCountX;
	unsigned int m_massCountY;
	unsigned int m_massCountZ;
	BodyData*** m_masses;

	float m_springConstant;
	float m_dampening;

	Mesh* m_mesh;
	std::unordered_map<std::tuple<unsigned int, unsigned int, unsigned int>, std::vector<Vertex*>> m_massToVertexMap;
};

