#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Scene* scene, Entity entity) : RenderComponent(scene, entity)
{
	m_mesh = nullptr;
}

MeshRenderComponent::~MeshRenderComponent()
{
}

Mesh* MeshRenderComponent::getMesh() const
{
	return m_mesh;
}

void MeshRenderComponent::setMesh(Mesh* mesh)
{
	m_mesh = mesh;
}
