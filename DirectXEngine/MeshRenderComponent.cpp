#include "MeshRenderComponent.h"

MeshRenderComponent::MeshRenderComponent(Scene* scene, unsigned int entity) : RenderComponent(scene, entity)
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

void MeshRenderComponent::changeMesh(Mesh* mesh)
{
	m_mesh = mesh;
}
