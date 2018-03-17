#pragma once

#include "../Component/ICollider.h"
#include "../Component/Rigidbody.h"

#include <DirectXMath.h>
#include <queue>

struct CollisionManifold
{
	Rigidbody* body1;
	Rigidbody* body2;
	DirectX::XMFLOAT3 collisionNormal;
	float penetrationDepth;
};

class PhysicsHandler
{
public:
	PhysicsHandler();
	~PhysicsHandler();

	void checkForCollisions(ICollider** colliders, unsigned int colliderCount);
	void resolveCollisions();

private:
	void broadPhaseDetection(ICollider** colliders, unsigned int colliderCount);
	void narrowPhaseDetection(ICollider** colliders, unsigned int colliderCount);

	std::queue<CollisionManifold> m_manifolds;
};