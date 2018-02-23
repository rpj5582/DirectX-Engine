#pragma once

#include "../Entity.h"
#include "../Asset/AssetManager.h"
#include "../Window.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "../Util.h"

#include <Windows.h>
#include <DirectXMath.h>

class Component
{
public:
	friend class Entity;

	virtual void init();
	virtual void initDebugVariables();
	virtual void update(float deltaTime, float totalTime);
	virtual void lateUpdate(float deltaTime, float totalTime);
	virtual void loadFromJSON(rapidjson::Value& dataObject);
	virtual void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	virtual void onSceneLoaded();

	Entity& getEntity() const;
	std::string getName() const;

	std::vector<DebugComponentData>& getDebugComponentData();

	bool enabled;
	
protected:
	Component(Entity& entity);
	virtual ~Component();

	Entity& entity;	
	std::string typeName;

	void debugAddInt(std::string label, int* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddUInt(std::string label, unsigned int* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddBool(std::string label, bool* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddFloat(std::string label, float* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddChar(std::string label, char* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddString(std::string label, std::string* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddVec2(std::string label, DirectX::XMFLOAT2* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddVec3(std::string label, DirectX::XMFLOAT3* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddVec4(std::string label, DirectX::XMFLOAT4* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddColor(std::string label, DirectX::XMFLOAT4* data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);

	void debugAddEnum(std::string label, void* data, unsigned int hash, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddStruct(std::string label, void* data, unsigned int hash, unsigned int stride, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);

	void debugAddTexture(std::string label, Texture** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddMaterial(std::string label, Material** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddModel(std::string label, Mesh** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddVertexShader(std::string label, VertexShader** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddPixelShader(std::string label, PixelShader** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddSampler(std::string label, Sampler** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);
	void debugAddFont(std::string label, Font** data, DebugGetterFunc getterFunc = nullptr, DebugSetterFunc setterFunc = nullptr);

private:
	std::vector<DebugComponentData> d_debugComponentData;
};