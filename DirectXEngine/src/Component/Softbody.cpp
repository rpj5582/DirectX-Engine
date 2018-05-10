#include "Softbody.h"

#include "Collider.h"
#include "MeshRenderComponent.h"

#include "../Input.h"

using namespace DirectX;

Softbody::Softbody(Entity& entity) : IPhysicsBody(entity)
{
	m_masses = nullptr;
	m_mesh = nullptr;

	m_size = XMFLOAT3();
	m_massCountX = 0;
	m_massCountY = 0;
	m_massCountZ = 0;

	m_springConstant = 100.0f;
	m_dampening = 1.0f;
}

Softbody::~Softbody()
{
	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			delete[] m_masses[i][j];
		}

		delete[] m_masses[i];
	}

	delete[] m_masses;
	m_masses = nullptr;
}

void Softbody::init()
{
	IPhysicsBody::init();

	m_size = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_massCountX = 4;
	m_massCountY = 4;
	m_massCountZ = 4;

	float startWidth = -m_size.x * 0.5f;
	float startHeight = -m_size.y * 0.5f;
	float startDepth = -m_size.z * 0.5f;

	float widthStep = m_size.x / m_massCountX;
	float heightStep = m_size.y / m_massCountY;
	float depthStep = m_size.z / m_massCountZ;

	m_masses = new BodyData**[m_massCountX];
	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		m_masses[i] = new BodyData*[m_massCountY];
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			m_masses[i][j] = new BodyData[m_massCountZ];
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				m_masses[i][j][k] = BodyData(XMFLOAT3(startWidth + widthStep * i, startHeight + heightStep * j, startDepth + depthStep * k), XMFLOAT3(), XMFLOAT3X3(), 1.0f, 0.0f);
			}
		}
	}

	Collider* collider = entity.getComponent<Collider>();
	if (collider)
	{
		Mesh* colliderMesh = collider->getMesh();
		m_mesh = AssetManager::createAsset<Mesh>("softCube", colliderMesh->getVertices(), colliderMesh->getVertexCount(), colliderMesh->getIndices(), colliderMesh->getIndexCount(), false);

		Vertex* vertices = m_mesh->getVertices();
		unsigned int vertexCount = m_mesh->getVertexCount();
		for (unsigned int n = 0; n < vertexCount; n++)
		{
			unsigned int i, j, k;
			getClosestBodyData(vertices[n].position, &i, &j, &k);

			m_massToVertexMap[std::tuple<unsigned int, unsigned int, unsigned int>(i, j, k)].push_back(&vertices[n]);
		}
	}

	if (m_mesh)
	{
		MeshRenderComponent* meshRenderComponent = entity.getComponent<MeshRenderComponent>();
		if (meshRenderComponent)
		{
			meshRenderComponent->setMesh(m_mesh);
		}
	}
}

void Softbody::initDebugVariables()
{
	IPhysicsBody::initDebugVariables();

	debugAddFloat("Dampening", &m_dampening);
	debugAddFloat("Spring Constant", &m_springConstant);
}

void Softbody::update(float deltaTime, float totalTime)
{
	/******TEST CODE******/

	XMFLOAT3 externalForce = XMFLOAT3();

	if (Input::isKeyDown(Keyboard::Up))
	{
		externalForce.z = 10.0f;
	}

	if (Input::isKeyDown(Keyboard::Down))
	{
		externalForce.z = -10.0f;
	}

	if (Input::isKeyDown(Keyboard::Right))
	{
		externalForce.x = 10.0f;
	}

	if (Input::isKeyDown(Keyboard::Left))
	{
		externalForce.x = -10.0f;
	}

	if (Input::isKeyDown(Keyboard::LeftShift))
	{
		externalForce.y = -50.0f;
	}

	/******END TEST CODE******/

	XMVECTOR displacement;
	XMVECTOR direction;
	float distance;

	XMVECTOR position;
	XMVECTOR neighborPosition;
	XMVECTOR velocity;

	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				XMVECTOR neighborForceVec = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				position = XMLoadFloat3(&m_masses[i][j][k].position);
				velocity = XMLoadFloat3(&m_masses[i][j][k].velocity);
				
				// Check for spring forces against neighbors

				// Neighbor to the left
				if (i > 0)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i - 1][j][k].position);
					displacement = XMVectorSubtract(neighborPosition, position);
					
					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}

					
					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountX))), XMVectorScale(velocity, m_dampening)));
				}

				// Neighbor to the right
				if (i < m_massCountX - 1)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i + 1][j][k].position);
					displacement = XMVectorSubtract(neighborPosition, position);

					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}


					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountX))), XMVectorScale(velocity, m_dampening)));
				}

				// Neighbor below
				if (j > 0)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i][j - 1][k].position);
					displacement = XMVectorSubtract(neighborPosition, position);

					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}


					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountY))), XMVectorScale(velocity, m_dampening)));
				}

				// Neighbor above
				if (j < m_massCountY - 1)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i][j + 1][k].position);
					displacement = XMVectorSubtract(neighborPosition, position);

					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}


					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountY))), XMVectorScale(velocity, m_dampening)));
				}

				// Neighbor behind
				if (k > 0)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i][j][k - 1].position);
					displacement = XMVectorSubtract(neighborPosition, position);

					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}


					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountZ))), XMVectorScale(velocity, m_dampening)));
				}

				// Neighbor in front
				if (k < m_massCountZ - 1)
				{
					neighborPosition = XMLoadFloat3(&m_masses[i][j][k + 1].position);
					displacement = XMVectorSubtract(neighborPosition, position);

					XMStoreFloat(&distance, XMVector3Length(displacement));

					if (distance > FLT_EPSILON)
						direction = XMVector3Normalize(displacement);
					else
					{
						direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
						distance = 0.0f;
					}


					neighborForceVec = XMVectorAdd(neighborForceVec, XMVectorSubtract(XMVectorScale(direction, m_springConstant * (distance - (1.0f / m_massCountZ))), XMVectorScale(velocity, m_dampening)));
				}

				if (j != m_massCountY - 1)
				{
					// Apply gravitational force to each body (except the top ones, so that it stretches like a slinky)
					applyForce({ 0.0f, -9.81f, 0.0f }, &m_masses[i][j][k]);

					XMFLOAT3 neighborForce;
					XMStoreFloat3(&neighborForce, neighborForceVec);
					applyForce(neighborForce, &m_masses[i][j][k]);
					applyForce(externalForce, &m_masses[i][j][k]);
				}
			}
		}
	}
}

void Softbody::integrateForces()
{
	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				m_masses[i][j][k].integrateForces();
			}
		}
	}
	
}

void Softbody::integrateVelocity()
{
	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				m_masses[i][j][k].integrateVelocity();
			}
		}
	}

	updateVisual();
}

BodyData* Softbody::getClosestBodyData(XMFLOAT3 point, unsigned int* i, unsigned int* j, unsigned int* k)
{
	// Find the closest body data given the collision point
	float min = FLT_MAX;
	BodyData* closestBodyData = nullptr;
	unsigned int x, y, z;
	XMVECTOR pointVec = XMLoadFloat3(&point);
	
	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				BodyData& bodyData = m_masses[i][j][k];
				XMVECTOR position = XMLoadFloat3(&bodyData.position);
				
				XMVECTOR distSq = XMVector3LengthSq(XMVectorSubtract(pointVec, position));
				float distanceSq;
				XMStoreFloat(&distanceSq, distSq);

				if (distanceSq < min)
				{
					min = distanceSq;
					closestBodyData = &bodyData;
					x = i;
					y = j;
					z = k;
				}
			}
		}
	}

	if(i)
		*i = x;

	if(j)
		*j = y;

	if (k)
		*k = z;

	return closestBodyData;
}

void Softbody::updateVisual()
{
	for (auto it = m_massToVertexMap.begin(); it != m_massToVertexMap.end(); it++)
	{
		unsigned int i = std::get<0>(it->first);
		unsigned int j = std::get<1>(it->first);
		unsigned int k = std::get<2>(it->first);
		
		for (unsigned int n = 0; n < it->second.size(); n++)
		{
			it->second[n]->position = m_masses[i][j][k].position;
		}
	}

	m_mesh->updateVertices();
}

void Softbody::applyForce(DirectX::XMFLOAT3 force, BodyData* body)
{
	XMVECTOR forceVec = XMLoadFloat3(&force);
	XMVECTOR totalForce = XMLoadFloat3(&body->totalForce);
	totalForce = XMVectorAdd(forceVec, totalForce);

	XMStoreFloat3(&body->totalForce, totalForce);
}

void Softbody::applyTorque(DirectX::XMFLOAT3 force, BodyData* body)
{
	XMVECTOR forceVec = XMLoadFloat3(&force);
	XMVECTOR pointVec = XMLoadFloat3(&body->position);

	XMFLOAT3 com = calculateCenterOfMass();
	XMVECTOR origin = XMLoadFloat3(&com);

	XMVECTOR totalTorque = XMLoadFloat3(&body->totalTorque);
	totalTorque = XMVectorAdd(XMVector3Cross(forceVec, XMVectorSubtract(pointVec, origin)), totalTorque);

	XMStoreFloat3(&body->totalTorque, totalTorque);
}

XMFLOAT3 Softbody::calculateCenterOfMass()
{
	XMVECTOR sum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < m_massCountX; i++)
	{
		for (unsigned int j = 0; j < m_massCountY; j++)
		{
			for (unsigned int k = 0; k < m_massCountZ; k++)
			{
				XMVECTOR position = XMLoadFloat3(&m_masses[i][j][k].position);
				sum = XMVectorAdd(sum, position);
			}
		}
	}

	sum = XMVectorScale(sum, 1.0f / (m_massCountX * m_massCountY * m_massCountZ));
	XMFLOAT3 com;
	XMStoreFloat3(&com, sum);

	return com;
}
