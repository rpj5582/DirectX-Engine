#pragma once
#include "ICollider.h"

class BoundingBox : public ICollider
{
public:
	BoundingBox(Entity& entity);
	~BoundingBox();

private:

};