#pragma once
#include "Component.h"

enum RenderStyle
{
	SOLID,
	WIREFRAME,
	SOLID_WIREFRAME
};

class RenderComponent : public Component
{
public:
	RenderComponent(Entity& entity);
	virtual ~RenderComponent();

	virtual void init() override;
	virtual void initDebugVariables() override;
	virtual void loadFromJSON(rapidjson::Value& dataObject) override;
	virtual void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) override;

	Material* getMaterial() const;
	std::string getMaterialID() const;
	void setMaterial(std::string materialID);

	RenderStyle getRenderStyle() const;
	void setRenderStyle(RenderStyle renderStyle);

private:
	std::string m_materialID;
	Material* m_material;

	RenderStyle m_renderStyle;

	static TwEnumVal d_renderStyleMembers[3];
	static TwType TW_TYPE_RENDER_STLYE;
};

void TW_CALL getRenderComponentMaterialDebugEditor(void* value, void* clientData);
void TW_CALL setRenderComponentMaterialDebugEditor(const void* value, void* clientData);
