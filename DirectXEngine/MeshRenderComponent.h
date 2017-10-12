#pragma once
#include "RenderComponent.h"

class MeshRenderComponent : public RenderComponent
{
public:
	MeshRenderComponent(Entity& entity);
	~MeshRenderComponent();

	virtual void loadFromJSON(rapidjson::Value& dataObject) override;

	Mesh* getMesh() const;
	void setMesh(Mesh* mesh);

private:
	Mesh* m_mesh;
};