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
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	Material* getMaterial() const;
	std::string getMaterialID() const;
	void setMaterial(std::string materialID);

	RenderStyle getRenderStyle() const;
	void setRenderStyle(RenderStyle renderStyle);

	DirectX::XMFLOAT4 getWireframeColor() const;
	void setWireframeColor(DirectX::XMFLOAT4 color);

private:
	std::string m_materialID;
	Material* m_material;

	RenderStyle m_renderStyle;
	DirectX::XMFLOAT4 m_wireColor;

	static TwEnumVal d_renderStyleMembers[3];
	static TwType TW_TYPE_RENDER_STLYE;
};

void TW_CALL getRenderComponentMaterialDebugEditor(void* value, void* clientData);
void TW_CALL setRenderComponentMaterialDebugEditor(const void* value, void* clientData);
