#pragma once
#include "RenderComponent.h"

class MeshRenderComponent : public RenderComponent
{
public:
	MeshRenderComponent(Entity& entity);
	~MeshRenderComponent();

	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	Mesh* getMesh() const;
	void setMesh(std::string meshID);

private:
	std::string m_meshID;
	Mesh* m_mesh;
};
