#pragma once
#include "RenderComponent.h"

class MeshRenderComponent : public RenderComponent
{
public:
	MeshRenderComponent(Entity& entity);
	~MeshRenderComponent();

	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	Mesh* getMesh() const;
	void setMesh(Mesh* mesh);

	bool castShadows;
	bool receiveShadows;

private:
	Mesh* m_mesh;
};
