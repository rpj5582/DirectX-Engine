#include "Collider.h"

#include "../Util.h"

using namespace DirectX;

// Most of the code here was adapted from http://www.dyn4j.org/2010/01/sat/ unless specified otherwise

Collider::Collider(Entity& entity) : Component(entity)
{
	m_collisionMesh = nullptr;

	m_faceNormals = std::vector<XMFLOAT3>();

	m_offset = XMFLOAT3();
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	updateOffsetScaleMatrix();
}

Collider::~Collider()
{
	if (!entity.getComponent<Collider>())
	{
		if (entity.hasTag(TAG_COLLIDER))
			entity.removeTag(TAG_COLLIDER);
	}
}

void Collider::init()
{
	Component::init();

	if (!entity.hasTag(TAG_COLLIDER))
		entity.addTag(TAG_COLLIDER);
}

void Collider::initDebugVariables()
{
	Component::initDebugVariables();

	debugAddModel("Model", &m_collisionMesh, &debugColliderGetMesh, &debugColliderSetMesh);
	debugAddVec3("Offset", &m_offset, &debugColliderGetOffset, &debugColliderSetOffset);
	debugAddVec3("Scale", &m_scale, &debugColliderGetScale, &debugColliderSetScale);
}

void Collider::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator mesh = dataObject.FindMember("mesh");
	if (mesh != dataObject.MemberEnd())
	{
		if (!mesh->value.IsNull())
			setMesh(AssetManager::getAsset<Mesh>(mesh->value.GetString()));
		else
			setMesh(nullptr);
	}

	rapidjson::Value::MemberIterator offset = dataObject.FindMember("offset");
	if (offset != dataObject.MemberEnd())
	{
		setOffset(XMFLOAT3(offset->value["x"].GetFloat(), offset->value["y"].GetFloat(), offset->value["z"].GetFloat()));
	}

	rapidjson::Value::MemberIterator scale = dataObject.FindMember("scale");
	if (scale != dataObject.MemberEnd())
	{
		setScale(XMFLOAT3(scale->value["x"].GetFloat(), scale->value["y"].GetFloat(), scale->value["z"].GetFloat()));
	}
}

void Collider::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	if (m_collisionMesh)
	{
		writer.Key("mesh");
		writer.String(m_collisionMesh->getAssetID().c_str());
	}
	else
	{
		writer.Key("mesh");
		writer.Null();
	}

	writer.Key("offset");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_offset.x);

	writer.Key("y");
	writer.Double(m_offset.y);

	writer.Key("z");
	writer.Double(m_offset.z);

	writer.EndObject();

	writer.Key("scale");
	writer.StartObject();

	writer.Key("x");
	writer.Double(m_scale.x);

	writer.Key("y");
	writer.Double(m_scale.y);

	writer.Key("z");
	writer.Double(m_scale.z);

	writer.EndObject();
}

bool Collider::calculateMTV(Collider& other, XMFLOAT3& mtv) const
{
	// If one of the colliders doesn't have a collision mesh attached, there can't be a collision
	if (!m_collisionMesh || !other.m_collisionMesh) return false;

	Transform* transform = entity.getComponent<Transform>();
	Transform* otherTransform = other.entity.getComponent<Transform>();

	if (!transform || !otherTransform) return false;

	std::vector<XMFLOAT3> mtvAxes;
	std::vector<float> mtvOverlaps;

	XMFLOAT4X4 worldMatrixFloat4x4 = transform->getWorldMatrix();
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

	XMFLOAT4X4 otherWorldMatrixFloat4x4 = otherTransform->getWorldMatrix();
	XMMATRIX otherWorldMatrix = XMLoadFloat4x4(&otherWorldMatrixFloat4x4);

	std::vector<XMFLOAT3> axes = std::vector<XMFLOAT3>(m_faceNormals.size());
	for (unsigned int i = 0; i < m_faceNormals.size(); i++)
	{
		XMVECTOR faceNormal = XMLoadFloat3(&m_faceNormals[i]);
		XMVECTOR transformedFaceNormal = XMVector3Normalize(XMVector3TransformNormal(faceNormal, worldMatrix));
		XMStoreFloat3(&axes[i], transformedFaceNormal);
	}

	std::vector<XMFLOAT3> otherAxes = std::vector<XMFLOAT3>(other.m_faceNormals.size());
	for (unsigned int i = 0; i < other.m_faceNormals.size(); i++)
	{
		XMVECTOR faceNormal = XMLoadFloat3(&other.m_faceNormals[i]);
		XMVECTOR transformedFaceNormal = XMVector3Normalize(XMVector3TransformNormal(faceNormal, otherWorldMatrix));
		XMStoreFloat3(&otherAxes[i], transformedFaceNormal);
	}

	// Check all axes from first collider
	for (unsigned int i = 0; i < axes.size(); i++)
	{
		XMFLOAT3 axis = axes[i];

		std::pair<float, float> projection = project(*transform, axis);
		std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

		// If the projections don't intersect, the colliders don't intersect
		if (otherProjection.first > projection.second || projection.first > otherProjection.second) return false;
		else
		{
			float overlapAmount = overlap(projection, otherProjection);
			if (overlapAmount > 0.0f)
			{
				mtvOverlaps.push_back(overlapAmount);
				mtvAxes.push_back(axis);
			}
		}
	}

	// Check all axes from second collider
	for (unsigned int i = 0; i < otherAxes.size(); i++)
	{
		XMFLOAT3 axis = otherAxes[i];

		std::pair<float, float> projection = project(*transform, axis);
		std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

		// If the projections don't intersect, the colliders don't intersect
		if (otherProjection.first > projection.second || projection.first > otherProjection.second) return false;
		else
		{
			float overlapAmount = overlap(projection, otherProjection);
			if (overlapAmount > 0.0f)
			{
				mtvOverlaps.push_back(overlapAmount);
				mtvAxes.push_back(axis);
			}
		}
	}

	// Check all axes generated from all pairs of axis cross products
	for (unsigned int i = 0; i < axes.size(); i++)
	{
		XMVECTOR axis1 = XMLoadFloat3(&axes[i]);
		for (unsigned int j = 0; j < otherAxes.size(); j++)
		{
			XMVECTOR axis2 = XMLoadFloat3(&otherAxes[j]);

			XMVECTOR cross = XMVector3Cross(axis1, axis2);
			float length;
			XMStoreFloat(&length, XMVector3Length(cross));
			if (length < FLT_EPSILON) continue;

			XMFLOAT3 axis;
			XMStoreFloat3(&axis, cross);

			std::pair<float, float> projection = project(*transform, axis);
			std::pair<float, float> otherProjection = other.project(*otherTransform, axis);

			// If the projections don't intersect, the colliders don't intersect
			if (otherProjection.first > projection.second || projection.first > otherProjection.second) return false;
			else
			{
				float overlapAmount = overlap(projection, otherProjection);
				if (overlapAmount > 0.0f)
				{
					mtvOverlaps.push_back(overlapAmount);
					mtvAxes.push_back(axis);
				}
			}
		}
	}

	float min = FLT_MAX;
	XMFLOAT3 mtvAxis;

	for (unsigned int i = 0; i < mtvOverlaps.size(); i++)
	{
		if (mtvOverlaps[i] < min)
		{
			min = mtvOverlaps[i];
			mtvAxis = mtvAxes[i];
		}
	}

	XMVECTOR mtvAxisVec = XMLoadFloat3(&mtvAxis);

	XMFLOAT3 otherPosition = otherTransform->getPosition();
	XMFLOAT3 position = transform->getPosition();

	XMVECTOR relativePosition = XMVectorSubtract(XMLoadFloat3(&otherPosition), XMLoadFloat3(&position));

	// If the length of the MTV is too small, treat it as zero
	float length;
	XMStoreFloat(&length, XMVector3Length(mtvAxisVec));
	if (length < FLT_EPSILON)
		return false;

	XMVECTOR dotResultVec = XMVector3Dot(relativePosition, mtvAxisVec);
	float dotResult;
	XMStoreFloat(&dotResult, dotResultVec);

	if (dotResult < 0.0f)
		mtvAxisVec = XMVectorScale(mtvAxisVec, -1.0f);

	mtvAxisVec = XMVectorScale(mtvAxisVec, min);
	XMStoreFloat3(&mtv, mtvAxisVec);

	// Account for floating point errors
	mtv.x = (fabs(mtv.x) < 2 * FLT_EPSILON) ? 0.0f : mtv.x;
	mtv.y = (fabs(mtv.y) < 2 * FLT_EPSILON) ? 0.0f : mtv.y;
	mtv.z = (fabs(mtv.z) < 2 * FLT_EPSILON) ? 0.0f : mtv.z;

	return true;
}

XMFLOAT3 Collider::calculateContactPoint(Collider& other, XMFLOAT3 mtvNormal)
{
	// Adapted from the Determining Point Of Collision 3D (Convex Hull) example in ATLAS
	XMFLOAT3 contactPoint = XMFLOAT3();

	XMFLOAT3 closestPoints[2], otherClosestPoints[2];

	int totalSuspects = 1, otherTotalSuspects = 1;

	Transform* transform = entity.getComponent<Transform>();
	Transform* otherTransform = other.entity.getComponent<Transform>();

	XMFLOAT4X4 worldMatrixFloat4x4 = transform->getWorldMatrix();
	XMFLOAT4X4 otherWorldMatrixFloat4x4 = otherTransform->getWorldMatrix();

	// Checks if there is a face to point collision where a point on this object is piercing a face of the other object.
	contactPoint = faceToPoint(worldMatrixFloat4x4, mtvNormal, closestPoints, totalSuspects, -FLT_MAX);
	if (contactPoint.x != 0 && contactPoint.y != 0 && contactPoint.z != 0)
	{
		return contactPoint;
	}

	// Checks if there is a face to point collision where a point on the other object is piercing a face of this object.
	contactPoint = other.faceToPoint(otherWorldMatrixFloat4x4, mtvNormal, otherClosestPoints, otherTotalSuspects, FLT_MAX);
	if (contactPoint.x != 0 && contactPoint.y != 0 && contactPoint.z != 0)
	{
		return contactPoint;
	}
	
	// Edge to edge collision
	if (totalSuspects == 2 && otherTotalSuspects == 2)
	{
		return edgeToEdge(worldMatrixFloat4x4, otherWorldMatrixFloat4x4, closestPoints, otherClosestPoints);
	}

	// Edge to face or face to face collision
	XMMATRIX otherWorldMatrix = XMLoadFloat4x4(&otherWorldMatrixFloat4x4);
	XMVECTOR d = XMVector3Dot(XMLoadFloat3(&mtvNormal), XMVector3Transform(XMLoadFloat3(&otherClosestPoints[0]), otherWorldMatrix));

	XMVECTOR v1, v2;
	XMVECTOR startingVec1 = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR startingVec2 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR mtvNormalVec = XMLoadFloat3(&mtvNormal);

	XMVECTOR closestPoint1 = XMLoadFloat3(&closestPoints[0]);
	XMVECTOR closestPoint2 = XMLoadFloat3(&closestPoints[1]);
	XMVECTOR otherClosestPoint1 = XMLoadFloat3(&otherClosestPoints[0]);
	XMVECTOR otherClosestPoint2 = XMLoadFloat3(&otherClosestPoints[1]);

	if (totalSuspects == 2)
	{
		startingVec1 = XMVector3Normalize(XMVectorSubtract(closestPoint1, closestPoint2));
		startingVec2 = XMVector3Cross(startingVec1, mtvNormalVec);
	}

	if (otherTotalSuspects == 2)
	{
		startingVec1 = XMVector3Normalize(XMVectorSubtract(otherClosestPoint1, otherClosestPoint2));
		startingVec2 = XMVector3Cross(startingVec1, mtvNormalVec);
	}

	v1 = XMVectorSubtract(startingVec1, XMVectorMultiply(mtvNormalVec, XMVector3Dot(mtvNormalVec, startingVec1)));
	float sqLength;
	XMStoreFloat(&sqLength, XMVector3LengthSq(v1));
	if (sqLength < 4.0f * FLT_EPSILON * FLT_EPSILON)
	{
		v1 = XMVectorSubtract(startingVec2, XMVectorMultiply(mtvNormalVec, XMVector3Dot(mtvNormalVec, startingVec1)));
		startingVec2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	}

	v2 = XMVectorSubtract(XMVectorSubtract(startingVec2, XMVectorMultiply(mtvNormalVec, XMVector3Dot(mtvNormalVec, startingVec2))), XMVectorMultiply(XMVectorMultiply(v1, mtvNormalVec), XMVector3Dot(v1, startingVec2)));
	XMStoreFloat(&sqLength, XMVector3LengthSq(v2));
	if (sqLength < 4.0f * FLT_EPSILON * FLT_EPSILON)
	{
		startingVec2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		v2 = XMVectorSubtract(XMVectorSubtract(startingVec2, XMVectorMultiply(mtvNormalVec, XMVector3Dot(mtvNormalVec, startingVec2))), XMVectorMultiply(XMVectorMultiply(v1, mtvNormalVec), XMVector3Dot(v1, startingVec2)));
	}

	v1 = XMVector3Normalize(v1);
	v2 = XMVector3Normalize(v2);

	XMFLOAT3 v1Float3;
	XMFLOAT3 v2Float3;
	XMStoreFloat3(&v1Float3, v1);
	XMStoreFloat3(&v2Float3, v2);

	XMFLOAT3 component1 = calculateContactPoint1D(*transform, *otherTransform, v1Float3);
	XMFLOAT3 component2 = calculateContactPoint1D(*transform, *otherTransform, v2Float3);

	XMVECTOR contactPointVec = XMVectorAdd(XMLoadFloat3(&component1), XMLoadFloat3(&component2));

	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVectorAdd(contactPointVec, XMVectorMultiply(mtvNormalVec, d)));

	return result;
}

Mesh* const Collider::getMesh() const
{
	return m_collisionMesh;
}

void Collider::setMesh(Mesh* const mesh)
{
	m_collisionMesh = mesh;
	if (m_collisionMesh)
		// Precompute the face normals, as each individual triangle needs to be evaluated
		m_faceNormals = calculateFaceNormals();
	else
		m_faceNormals.clear();
}

DirectX::XMFLOAT3 Collider::getOffset() const
{
	return m_offset;
}

DirectX::XMFLOAT3 Collider::getScale() const
{
	return m_scale;
}

DirectX::XMFLOAT4X4 Collider::getOffsetScaleMatrix() const
{
	return m_offsetScaleMatrix;
}

void Collider::setOffset(DirectX::XMFLOAT3 offset)
{
	m_offset = offset;
	updateOffsetScaleMatrix();
}

void Collider::setScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	updateOffsetScaleMatrix();
}

std::vector<XMFLOAT3> Collider::calculateFaceNormals() const
{
	// Adapted from https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal (Newell's Method)

	std::vector<XMFLOAT3> normals = std::vector<XMFLOAT3>();

	const Vertex* vertices = m_collisionMesh->getVertices();
	const unsigned int* indices = m_collisionMesh->getIndices();
	unsigned int indexCount = m_collisionMesh->getIndexCount();

	for (unsigned int i = 0; i < indexCount;)
	{
		XMVECTOR normal = XMVectorSet(0, 0, 0, 0);

		for (unsigned int j = 0; j < 3; j++)
		{
			unsigned int currentIndex = indices[i + j];
			unsigned int nextIndex = indices[i + ((j + 1) % 3)];

			// This is a SIMD adaptation of the following code:

			/*normal.x += (currentVertex.y - nextVertex.y) * (currentVertex.z + nextVertex.z);
			normal.y += (currentVertex.z - nextVertex.z) * (currentVertex.x + nextVertex.x);
			normal.z += (currentVertex.x - nextVertex.x) * (currentVertex.y + nextVertex.y);*/

			XMFLOAT3 currentVertex = vertices[currentIndex].position;
			XMFLOAT3 nextVertex = vertices[nextIndex].position;

			XMVECTOR current = XMLoadFloat3(&currentVertex);
			XMVECTOR next = XMLoadFloat3(&nextVertex);

			XMVECTOR sub = XMVectorSubtract(current, next);
			XMVECTOR sum = XMVectorAdd(current, next);

			XMFLOAT3 subFloat3;
			XMFLOAT3 sumFloat3;
			XMStoreFloat3(&subFloat3, sub);
			XMStoreFloat3(&sumFloat3, sum);

			XMFLOAT3 modfiedSub = XMFLOAT3(subFloat3.y, subFloat3.z, subFloat3.x);
			XMFLOAT3 modfiedSum = XMFLOAT3(sumFloat3.z, sumFloat3.x, sumFloat3.y);

			sub = XMLoadFloat3(&modfiedSub);
			sum = XMLoadFloat3(&modfiedSum);

			normal = XMVectorMultiplyAdd(sub, sum, normal);
		}

		XMFLOAT3 finalNormal;
		XMStoreFloat3(&finalNormal, XMVector3Normalize(normal));

		// Check to see if we already have this normal or the opposite normal, if we do, we don't need to keep it
		bool keepNormal = true;
		for (unsigned int j = 0; j < normals.size(); j++)
		{
			XMFLOAT3 oldNormal = normals[j];

			if (oldNormal.x == finalNormal.x && oldNormal.y == finalNormal.y && oldNormal.z == finalNormal.z || 
				oldNormal.x == -finalNormal.x && oldNormal.y == -finalNormal.y && oldNormal.z == -finalNormal.z)
			{
				keepNormal = false;
				break;
			}
		}

		if(keepNormal)
			normals.push_back(finalNormal);

		i += 3;
	}

	return normals;
}

std::pair<float, float> Collider::project(Transform& transform, XMFLOAT3 axis) const
{
	XMFLOAT4X4 worldMatrixFloat4x4 = transform.getWorldMatrix();
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);

	XMMATRIX offsetScaleMatrix = XMLoadFloat4x4(&m_offsetScaleMatrix);

	XMVECTOR axisVec = XMLoadFloat3(&axis);

	float min = FLT_MAX;
	float max = -FLT_MAX;

	const Vertex* vertices = m_collisionMesh->getVertices();
	unsigned int vertexCount = m_collisionMesh->getVertexCount();

	for (unsigned int i = 0; i < vertexCount; i++)
	{
		XMVECTOR vertex = XMLoadFloat3(&vertices[i].position);
		XMVECTOR vertexOffsetScaled = XMVector3TransformCoord(vertex, offsetScaleMatrix);
		XMVECTOR dotResult = XMVector3Dot(XMVector3TransformCoord(vertexOffsetScaled, worldMatrix), axisVec);

		float projection;
		XMStoreFloat(&projection, dotResult);

		if (projection < min)
			min = projection;
		else if (projection > max)
			max = projection;
	}
	
	return std::pair<float, float>(min, max);
}

float Collider::overlap(std::pair<float, float> projection, std::pair<float, float> otherProjection) const
{
	// Start out assuming there's no overlap
	float overlap = 0.0f;

	// Case 1 : min1 lies on the left of min2
	if (projection.first < otherProjection.first)
	{
		if (projection.second >= otherProjection.second)
			overlap = otherProjection.second - otherProjection.first;
		else
			overlap = projection.second - otherProjection.first;
	}

	// Case 2 : min1 lies between min2 and max2
	if (projection.first >= otherProjection.first && projection.first < otherProjection.second)
	{
		if (projection.second <= otherProjection.second)
			overlap = projection.second - projection.first;
		else
			overlap = otherProjection.second - projection.first;
	}

	// Case 3 : min1 lies outside max2 - overlap is already 0, so there's no need for this check
	//if (projection.first >= otherProjection.second)
		//overlap = 0.0f;

	return max(0.0f, overlap); // Don't return negative overlap
}

XMFLOAT3 Collider::faceToPoint(const DirectX::XMFLOAT4X4& worldMatrixFloat4x4, const DirectX::XMFLOAT3& mtvNormal, XMFLOAT3* closestPoints, int& totalSuspects, float startingInf)
{
	bool lessThan = true;
	if (startingInf > 0.0f)
	{
		lessThan = false;
	}

	XMFLOAT3 contactPoint = XMFLOAT3();

	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);
	XMVECTOR mtvNormalVec = XMLoadFloat3(&mtvNormal);

	std::unordered_set<XMFLOAT3> vertexPositions = std::unordered_set<XMFLOAT3>();

	const Vertex* vertices = m_collisionMesh->getVertices();
	unsigned int vertexCount = m_collisionMesh->getVertexCount();
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		if (vertexPositions.find(vertices[i].position) != vertexPositions.end())
			continue;
		else
			vertexPositions.insert(vertices[i].position);

		XMVECTOR vertexPosition = XMLoadFloat3(&vertices[i].position);

		XMVECTOR pointVec = XMVector3Dot(XMVector3TransformCoord(vertexPosition, worldMatrix), mtvNormalVec);
		float point;
		XMStoreFloat(&point, pointVec);

		if (fabs(startingInf - point) <= 0.001f)
		{
			closestPoints[1] = vertices[i].position;
			totalSuspects++;
		}
		else if (lessThan && startingInf < point)
		{
			startingInf = point;
			totalSuspects = 1;
			contactPoint = vertices[i].position;
			closestPoints[0] = contactPoint;
			closestPoints[1] = contactPoint;
		}
		else if (!lessThan && startingInf > point)
		{
			startingInf = point;
			totalSuspects = 1;
			contactPoint = vertices[i].position;
			closestPoints[0] = contactPoint;
			closestPoints[1] = contactPoint;
		}
	}

	if (totalSuspects == 1)
	{
		XMVECTOR contactPointVec = XMLoadFloat3(&contactPoint);
		XMVECTOR contactPointVecWorld = XMVector3TransformCoord(contactPointVec, worldMatrix);

		XMFLOAT3 result;
		XMStoreFloat3(&result, contactPointVecWorld);

		return result;
	}

	return XMFLOAT3();
}

DirectX::XMFLOAT3 Collider::edgeToEdge(const DirectX::XMFLOAT4X4& worldMatrixFloat4x4, const DirectX::XMFLOAT4X4& otherWorldMatrixFloat4x4, DirectX::XMFLOAT3* closestPoints, DirectX::XMFLOAT3* otherClosestPoints)
{
	XMMATRIX worldMatrix = XMLoadFloat4x4(&worldMatrixFloat4x4);
	XMMATRIX otherWorldMatrix = XMLoadFloat4x4(&otherWorldMatrixFloat4x4);

	XMFLOAT3 closestPoint1;
	XMStoreFloat3(&closestPoint1, XMVector3TransformCoord(XMLoadFloat3(&closestPoints[0]), worldMatrix));

	XMFLOAT3 closestPoint2;
	XMStoreFloat3(&closestPoint2, XMVector3TransformCoord(XMLoadFloat3(&closestPoints[1]), worldMatrix));

	XMFLOAT3 otherClosestPoint1;
	XMStoreFloat3(&otherClosestPoint1, XMVector3TransformCoord(XMLoadFloat3(&closestPoints[0]), otherWorldMatrix));

	XMFLOAT3 otherClosestPoint2;
	XMStoreFloat3(&otherClosestPoint2, XMVector3TransformCoord(XMLoadFloat3(&closestPoints[1]), otherWorldMatrix));

	std::pair<XMFLOAT3, XMFLOAT3> line = std::pair<XMFLOAT3, XMFLOAT3>(closestPoint1, closestPoint2);
	std::pair<XMFLOAT3, XMFLOAT3> otherLine = std::pair<XMFLOAT3, XMFLOAT3>(otherClosestPoint1, otherClosestPoint2);

	XMVECTOR d1, d2, r;

	XMVECTOR l1 = XMLoadFloat3(&line.first);
	XMVECTOR l2 = XMLoadFloat3(&line.second);
	XMVECTOR ol1 = XMLoadFloat3(&otherLine.first);
	XMVECTOR ol2 = XMLoadFloat3(&otherLine.second);

	d1 = XMVectorSubtract(l2, l1);
	d2 = XMVectorSubtract(ol2, ol1);
	r = XMVectorSubtract(l1, ol1);

	XMVECTOR a = XMVector3Dot(d1, d1);
	XMVECTOR b = XMVector3Dot(d1, d2);
	XMVECTOR c = XMVector3Dot(d1, r);
	XMVECTOR e = XMVector3Dot(d2, d2);
	XMVECTOR f = XMVector3Dot(d2, r);

	XMVECTOR d = XMVectorSubtract(XMVectorMultiply(a, e), XMVectorMultiply(b, b));

	XMVECTOR s = XMVectorDivide(XMVectorSubtract(XMVectorMultiply(b, f), XMVectorMultiply(c, e)), d);
	XMVECTOR t = XMVectorDivide(XMVectorSubtract(XMVectorMultiply(a, f), XMVectorMultiply(b, c)), d);

	XMVECTOR p1, p2;
	p1 = XMVectorAdd(l1, XMVectorMultiply(s, d1));
	p2 = XMVectorAdd(ol1, XMVectorMultiply(t, d2));

	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVectorScale(XMVectorAdd(p1, p2), 0.5f));

	return result;
}

DirectX::XMFLOAT3 Collider::calculateContactPoint1D(Transform& transform, Transform& otherTransform, const DirectX::XMFLOAT3 axis)
{
	XMFLOAT3 contactPoint;
	XMVECTOR mtvNormalVec = XMLoadFloat3(&axis);

	std::pair<float, float> projection = project(transform, axis);
	std::pair<float, float> otherProjection = project(otherTransform, axis);

	float o = overlap(projection, otherProjection);

	if (projection.first < otherProjection.first)
	{
		XMVECTOR contactPointVec = XMVectorScale(mtvNormalVec, otherProjection.first + (o * 0.5f));
		XMStoreFloat3(&contactPoint, contactPointVec);
	}
	else
	{
		XMVECTOR contactPointVec = XMVectorScale(mtvNormalVec, projection.first + (o * 0.5f));
		XMStoreFloat3(&contactPoint, contactPointVec);
	}

	return contactPoint;
}

void Collider::updateOffsetScaleMatrix()
{
	XMVECTOR offset = XMLoadFloat3(&m_offset);
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
	XMMATRIX offsetMatrix = XMMatrixTranslationFromVector(offset);
	
	XMMATRIX offsetScaleMatrix = XMMatrixMultiply(scaleMatrix, offsetMatrix);

	XMStoreFloat4x4(&m_offsetScaleMatrix, offsetScaleMatrix);
}

void debugColliderGetMesh(const Component* component, void* value)
{
	const Mesh* mesh = static_cast<const Collider*>(component)->getMesh();
	*static_cast<const Mesh**>(value) = mesh;
}

void debugColliderSetMesh(Component* component, const void* value)
{
	Mesh* mesh = *static_cast<Mesh*const*>(value);
	static_cast<Collider*>(component)->setMesh(mesh);
}

void debugColliderGetOffset(const Component* component, void* value)
{
	XMFLOAT3 offset = static_cast<const Collider*>(component)->getOffset();
	*static_cast<XMFLOAT3*>(value) = offset;
}

void debugColliderSetOffset(Component* component, const void* value)
{
	const XMFLOAT3 offset = *static_cast<const XMFLOAT3*>(value);
	static_cast<Collider*>(component)->setOffset(offset);
}

void debugColliderGetScale(const Component* component, void* value)
{
	XMFLOAT3 scale = static_cast<const Collider*>(component)->getScale();
	*static_cast<XMFLOAT3*>(value) = scale;
}

void debugColliderSetScale(Component* component, const void* value)
{
	const XMFLOAT3 scale = *static_cast<const XMFLOAT3*>(value);
	static_cast<Collider*>(component)->setScale(scale);
}
