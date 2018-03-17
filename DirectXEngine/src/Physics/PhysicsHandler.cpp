#include "PhysicsHandler.h"

using namespace DirectX;

PhysicsHandler::PhysicsHandler()
{
}

PhysicsHandler::~PhysicsHandler()
{
}

void PhysicsHandler::checkForCollisions(ICollider** colliders, unsigned int colliderCount)
{
	broadPhaseDetection(colliders, colliderCount);
	narrowPhaseDetection(colliders, colliderCount);
}

void PhysicsHandler::resolveCollisions()
{
	// Adapted from https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

	while (m_manifolds.size() > 0)
	{
		const CollisionManifold& manifold = m_manifolds.front();
		
		if (manifold.body1 && manifold.body2)
		{
			float inverseMass1 = manifold.body1->getInverseMass();
			float inverseMass2 = manifold.body2->getInverseMass();

			if (inverseMass1 > 0 || inverseMass2 > 0)
			{
				XMVECTOR collisionNormal = XMLoadFloat3(&manifold.collisionNormal);

				XMFLOAT3 velocity1 = manifold.body1->getVelocity();
				XMVECTOR vel1 = XMLoadFloat3(&velocity1);

				XMFLOAT3 velocity2 = manifold.body2->getVelocity();
				XMVECTOR vel2 = XMLoadFloat3(&velocity2);

				XMVECTOR relativeVelocity = XMVectorSubtract(vel2, vel1);
				float dotResult;
				XMStoreFloat(&dotResult, XMVector3Dot(relativeVelocity, collisionNormal));

				// Don't resolve collisions on bodies that are moving away from each other - they will resolve themselves
				if (dotResult <= 0)
				{
					float restitutionFactor = -1.0f - min(manifold.body1->getRestitution(), manifold.body2->getRestitution());
					float impulseMagnitude = (restitutionFactor * dotResult) / (inverseMass1 + inverseMass2);

					XMVECTOR impulseVec = XMVectorScale(collisionNormal, impulseMagnitude);
					XMVECTOR negativeImpulseVec = XMVectorNegate(impulseVec);

					XMFLOAT3 impulse;
					XMStoreFloat3(&impulse, impulseVec);

					XMFLOAT3 negativeImpulse;
					XMStoreFloat3(&negativeImpulse, negativeImpulseVec);

					manifold.body1->applyImpulse(negativeImpulse);
					manifold.body2->applyImpulse(impulse);

					manifold.body1->integrateVelocity();
					manifold.body2->integrateVelocity();

					// Positional correction
					const float correctionPercent = 0.1f;
					const float slop = 0.05f;
					float correctionMag = (max(manifold.penetrationDepth - slop, 0.0f) / (inverseMass1 + inverseMass2)) * correctionPercent;
					XMVECTOR correctionVec = XMVectorScale(collisionNormal, correctionMag);

					XMFLOAT3 correction1;
					XMStoreFloat3(&correction1, XMVectorScale(correctionVec, -inverseMass1));

					XMFLOAT3 correction2;
					XMStoreFloat3(&correction2, XMVectorScale(correctionVec, inverseMass2));

					Transform* transform1 = manifold.body1->getEntity().getComponent<Transform>();
					Transform* transform2 = manifold.body2->getEntity().getComponent<Transform>();

					if (transform1) transform1->move(correction1);
					if (transform2) transform2->move(correction2);
				}
			}
		}

		m_manifolds.pop();
	}
}

void PhysicsHandler::broadPhaseDetection(ICollider** colliders, unsigned int colliderCount)
{
	for (unsigned int i = 0; i < colliderCount; i++)
	{
		ICollider* collider1 = colliders[i];
		Rigidbody* body1 = collider1->getEntity().getComponent<Rigidbody>();

		for (unsigned int j = 0; j < colliderCount; j++)
		{
			ICollider* collider2 = colliders[j];
			Rigidbody* body2 = collider2->getEntity().getComponent<Rigidbody>();

			if (collider1 != collider2)
			{
				XMFLOAT3 mtv = collider1->doSAT(*collider2);
				if (mtv.x != 0 || mtv.y != 0 || mtv.z != 0)
				{
					XMVECTOR mtvVec = XMLoadFloat3(&mtv);
					XMVECTOR collisionNormalVec = XMVector3Normalize(mtvVec);
					XMVECTOR penetrationDepthVec = XMVector3Length(mtvVec);

					XMFLOAT3 collisionNormal;
					float penetrationDepth;
					XMStoreFloat3(&collisionNormal, collisionNormalVec);
					XMStoreFloat(&penetrationDepth, penetrationDepthVec);

					m_manifolds.push({ body1, body2, collisionNormal, penetrationDepth });
				}
			}
		}
	}
}

void PhysicsHandler::narrowPhaseDetection(ICollider** colliders, unsigned int colliderCount)
{
}
