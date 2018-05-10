#include "PhysicsHandler.h"

using namespace DirectX;

PhysicsHandler::PhysicsHandler()
{
}

PhysicsHandler::~PhysicsHandler()
{
}

void PhysicsHandler::checkForCollisions(Collider** colliders, unsigned int colliderCount)
{
	broadPhaseDetection(colliders, colliderCount);
	narrowPhaseDetection(colliders, colliderCount);
}

void PhysicsHandler::resolveCollisions()
{
	// Adapted from https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
	// and the 3D Convex Hull Collision Resolution tutorial in ATLAS
	while (m_manifolds.size() > 0)
	{
		const CollisionManifold& manifold = m_manifolds.front();

		BodyData* bodyData1 = manifold.body1->getClosestBodyData(manifold.contactPoint);
		BodyData* bodyData2 = manifold.body2->getClosestBodyData(manifold.contactPoint);

		if (bodyData1->invMass > 0 || bodyData2->invMass > 0)
		{
			XMVECTOR collisionNormal = XMLoadFloat3(&manifold.collisionNormal);

			XMVECTOR vel1 = XMLoadFloat3(&bodyData1->velocity);
			XMVECTOR vel2 = XMLoadFloat3(&bodyData2->velocity);

			XMVECTOR contactPoint = XMLoadFloat3(&manifold.contactPoint);

			XMVECTOR pos1 = XMLoadFloat3(&bodyData1->position);
			XMVECTOR pos2 = XMLoadFloat3(&bodyData2->position);

			XMVECTOR radius1 = XMVectorSubtract(contactPoint, pos1);
			XMVECTOR radius2 = XMVectorSubtract(contactPoint, pos2);

			XMVECTOR angVel1 = XMLoadFloat3(&bodyData1->angularVelocity);
			XMVECTOR angVel2 = XMLoadFloat3(&bodyData2->angularVelocity);

			XMVECTOR contactPointVelocity1 = XMVectorAdd(vel1, XMVector3Cross(angVel1, radius1));
			XMVECTOR contactPointVelocity2 = XMVectorAdd(vel2, XMVector3Cross(angVel2, radius2));

			XMVECTOR relativeVelocity = XMVectorSubtract(contactPointVelocity1, contactPointVelocity2);
			float dotResult;
			XMStoreFloat(&dotResult, XMVector3Dot(relativeVelocity, collisionNormal));

			// Don't resolve collisions on bodies that are moving away from each other - they will resolve themselves
			if (dotResult > 0.0f)
			{
				float restitutionFactor = -1.0f - min(bodyData1->restitution, bodyData2->restitution);

				XMMATRIX inertia1 = XMLoadFloat3x3(&bodyData1->inertia);
				XMMATRIX inertia2 = XMLoadFloat3x3(&bodyData2->inertia);

				XMMATRIX rotationMat1 = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&bodyData1->rotation));
				XMMATRIX rotationMat1T = XMMatrixTranspose(rotationMat1);

				XMMATRIX rotationMat2 = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&bodyData2->rotation));
				XMMATRIX rotationMat2T = XMMatrixTranspose(rotationMat2);

				inertia1 = XMMatrixMultiply(XMMatrixMultiply(rotationMat1, inertia1), rotationMat1T);
				inertia2 = XMMatrixMultiply(XMMatrixMultiply(rotationMat2, inertia2), rotationMat2T);

				XMVECTOR determinant;

				XMMATRIX invInertia1 = XMMatrixInverse(&determinant, inertia1);
				XMVECTOR isInverseInvalid = XMVectorEqual(determinant, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
				float inverseInvalid;
				XMStoreFloat(&inverseInvalid, isInverseInvalid);

				if (inverseInvalid)
				{
					invInertia1 = XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				}

				XMMATRIX invInertia2 = XMMatrixInverse(&determinant, inertia2);
				isInverseInvalid = XMVectorEqual(determinant, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
				XMStoreFloat(&inverseInvalid, isInverseInvalid);

				if (inverseInvalid)
				{
					invInertia2 = XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				}

				XMVECTOR denom =
					XMVector3Dot(
						XMVectorAdd(
							XMVector3Cross(XMVector3Transform(XMVector3Cross(radius1, collisionNormal), invInertia1), radius1),
							XMVector3Cross(XMVector3Transform(XMVector3Cross(radius2, collisionNormal), invInertia2), radius2)),
						collisionNormal);

				float denomComponent;
				XMStoreFloat(&denomComponent, denom);

				float impulseMagnitude = (restitutionFactor * dotResult) / (bodyData1->invMass + bodyData2->invMass + denomComponent);

				XMVECTOR impulseVec = XMVectorScale(collisionNormal, impulseMagnitude);
				XMVECTOR negativeImpulseVec = XMVectorNegate(impulseVec);

				XMVECTOR v1 = XMVectorScale(impulseVec, bodyData1->invMass);
				XMStoreFloat3(&bodyData1->velocity, XMVectorAdd(vel1, v1));

				XMVECTOR v2 = XMVectorScale(negativeImpulseVec, bodyData2->invMass);
				XMStoreFloat3(&bodyData2->velocity, XMVectorAdd(vel2, v2));

				XMVECTOR angularMomentum1 = XMVectorScale(XMVector3Cross(radius1, collisionNormal), impulseMagnitude);
				XMVECTOR angularMomentum2 = XMVectorScale(XMVector3Cross(radius2, collisionNormal), -impulseMagnitude);

				angVel1 = XMVectorAdd(angVel1, XMVector3Transform(angularMomentum1, invInertia1));
				XMStoreFloat3(&bodyData1->angularVelocity, angVel1);

				bodyData1->angularVelocity.x = (fabs(bodyData1->angularVelocity.x) < FLT_EPSILON) ? 0.0f : bodyData1->angularVelocity.x;
				bodyData1->angularVelocity.y = (fabs(bodyData1->angularVelocity.y) < FLT_EPSILON) ? 0.0f : bodyData1->angularVelocity.y;
				bodyData1->angularVelocity.z = (fabs(bodyData1->angularVelocity.z) < FLT_EPSILON) ? 0.0f : bodyData1->angularVelocity.z;

				angVel2 = XMVectorAdd(angVel2, XMVector3Transform(angularMomentum2, invInertia2));
				XMStoreFloat3(&bodyData2->angularVelocity, angVel2);

				bodyData2->angularVelocity.x = (fabs(bodyData2->angularVelocity.x) < FLT_EPSILON) ? 0.0f : bodyData2->angularVelocity.x;
				bodyData2->angularVelocity.y = (fabs(bodyData2->angularVelocity.y) < FLT_EPSILON) ? 0.0f : bodyData2->angularVelocity.y;
				bodyData2->angularVelocity.z = (fabs(bodyData2->angularVelocity.z) < FLT_EPSILON) ? 0.0f : bodyData2->angularVelocity.z;

				bodyData1->integrateVelocity();
				bodyData2->integrateVelocity();

				// Positional correction
				pos1 = XMLoadFloat3(&bodyData1->position);
				pos2 = XMLoadFloat3(&bodyData2->position);

				const float correctionPercent = 0.1f;
				const float slop = 0.001f;
				float correctionMag = (max(manifold.penetrationDepth - slop, 0.0f) / (bodyData1->invMass + bodyData2->invMass)) * correctionPercent;
				XMVECTOR correctionVec = XMVectorScale(collisionNormal, correctionMag);

				XMStoreFloat3(&bodyData1->position, XMVectorAdd(pos1, XMVectorScale(correctionVec, -bodyData1->invMass)));
				XMStoreFloat3(&bodyData2->position, XMVectorAdd(pos2, XMVectorScale(correctionVec, bodyData2->invMass)));

				manifold.body1->updateVisual();
				manifold.body2->updateVisual();
			}
		}

		m_manifolds.pop();
	}
}

void PhysicsHandler::broadPhaseDetection(Collider** colliders, unsigned int colliderCount)
{
	for (unsigned int i = 0; i < colliderCount; i++)
	{
		Collider* collider1 = colliders[i];
		IPhysicsBody* body1 = collider1->getEntity().getComponent<IPhysicsBody>();
		Transform* transform1 = collider1->getEntity().getComponent<Transform>();

		for (unsigned int j = 0; j < colliderCount; j++)
		{
			Collider* collider2 = colliders[j];
			IPhysicsBody* body2 = collider2->getEntity().getComponent<IPhysicsBody>();
			Transform* transform2 = collider2->getEntity().getComponent<Transform>();

			if (collider1 != collider2 && transform1 && transform2)
			{
				XMFLOAT3 mtv;
				if (collider1->calculateMTV(*collider2, mtv))
				{
					XMVECTOR mtvVec = XMLoadFloat3(&mtv);
					XMVECTOR collisionNormalVec = XMVector3Normalize(mtvVec);
					XMVECTOR penetrationDepthVec = XMVector3Length(mtvVec);

					XMFLOAT3 collisionNormal;
					float penetrationDepth;
					XMStoreFloat3(&collisionNormal, collisionNormalVec);
					XMStoreFloat(&penetrationDepth, penetrationDepthVec);

					XMFLOAT3 contactPoint = collider1->calculateContactPoint(*collider2, collisionNormal);

					if(body1 && body2)
						m_manifolds.push({ contactPoint, collisionNormal, penetrationDepth, body1, body2 });
				}
			}
		}
	}
}

void PhysicsHandler::narrowPhaseDetection(Collider** colliders, unsigned int colliderCount)
{
	// Currently unimplemented, all collision detection code is under the broad phase function
}
