#pragma once
#include "RenderComponent.h"

class MeshRenderComponent : public RenderComponent
{
public:
	MeshRenderComponent(Entity& entity);
	~MeshRenderComponent();

	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	Mesh* getMesh() const;
	std::string getMeshID() const;
	void setMesh(std::string meshID);

	bool castShadows;
	bool receiveShadows;

private:
	std::string m_meshID;
	Mesh* m_mesh;
};

void TW_CALL getMeshRenderComponentMeshDebugEditor(void* value, void* clientData);
void TW_CALL setMeshRenderComponentMeshDebugEditor(const void* value, void* clientData);
