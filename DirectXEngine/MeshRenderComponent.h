#pragma once
#include "RenderComponent.h"

class MeshRenderComponent : public RenderComponent
{
public:
	MeshRenderComponent(Scene* scene, Entity entity);
	~MeshRenderComponent();

	Mesh* getMesh() const;
	void setMesh(Mesh* mesh);

private:
	Mesh* m_mesh;
};