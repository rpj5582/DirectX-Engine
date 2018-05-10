#pragma once

#include "../Component/Collider.h"
#include "../Component/IPhysicsBody.h"

#include <DirectXMath.h>
#include <queue>

struct CollisionManifold
{
	DirectX::XMFLOAT3 contactPoint;
	DirectX::XMFLOAT3 collisionNormal;
	float penetrationDepth;
	IPhysicsBody* body1;
	IPhysicsBody* body2;
};

class PhysicsHandler
{
public:
	PhysicsHandler();
	~PhysicsHandler();

	void checkForCollisions(Collider** colliders, unsigned int colliderCount);
	void resolveCollisions();

private:
	void broadPhaseDetection(Collider** colliders, unsigned int colliderCount);
	void narrowPhaseDetection(Collider** colliders, unsigned int colliderCount);

	std::queue<CollisionManifold> m_manifolds;
};