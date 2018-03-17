#include "BoundingBox.h"

using namespace DirectX;

//XMFLOAT3 front[] =
//{ 
//	{ -0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, 0.5f },
//	{ 0.5f, 0.5f, 0.5f },
//	{ -0.5f, 0.5f, 0.5f }
//};
//
//XMFLOAT3 left[] = 
//{
//	{ -0.5f, -0.5f, -0.5f },
//	{ -0.5f, -0.5f, 0.5f },
//	{ -0.5f, 0.5f, 0.5f },
//	{ -0.5f, 0.5f, -0.5f }
//};
//
//XMFLOAT3 back[] = 
//{
//	{ 0.5f, -0.5f, -0.5f },
//	{ -0.5f, -0.5f, -0.5f },
//	{ -0.5f, 0.5f, -0.5f },
//	{ 0.5f, 0.5f, -0.5f }
//};
//
//XMFLOAT3 right[] =
//{
//	{ 0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, -0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//	{ 0.5f, 0.5f, 0.5f }
//};
//
//XMFLOAT3 top[] = 
//{
//	{ -0.5f, 0.5f, 0.5f },
//	{ 0.5f, 0.5f, 0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//	{ -0.5f, 0.5f, -0.5f },
//};
//
//XMFLOAT3 bottom[] = 
//{
//	{ -0.5f, -0.5f, -0.5f },
//	{ 0.5f, -0.5f, -0.5f },
//	{ 0.5f, -0.5f, 0.5f },
//	{ -0.5f, -0.5f, 0.5f },
//};
//
//ColliderPolygon polygons[] = 
//{
//	{ front, 4 },
//	{ left, 4 },
//	{ back, 4 },
//	{ right, 4 },
//	{ top, 4 },
//	{ bottom, 4 }
//};

BoundingBox::BoundingBox(Entity& entity) : ICollider(entity, AssetManager::getAsset<Mesh>(DEFAULT_MODEL_CUBE))
{
}

BoundingBox::~BoundingBox()
{
}
