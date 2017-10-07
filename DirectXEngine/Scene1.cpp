#include "Scene1.h"

using namespace DirectX;

Scene1::Scene1(ID3D11Device* device, ID3D11DeviceContext* context) : Scene(device, context)
{
}

Scene1::~Scene1()
{
}

bool Scene1::init()
{
	if (!Scene::init()) return false;

	AssetManager::loadTexture("cracks", "cracks.png");
	AssetManager::loadTexture("cracks_spec", "cracks_spec.png");
	AssetManager::loadTexture("cracks_norm", "cracks_norm.png");
	Material* cracksMaterial = AssetManager::createMaterial("cracks", "cracks", "cracks_spec", "cracks_norm");
	Mesh* cube = AssetManager::loadMesh("cube", "cube.obj");
	Mesh* torus = AssetManager::loadMesh("torus", "torus.obj");
	Mesh* cone = AssetManager::loadMesh("cone", "cone.obj");

	unsigned int cameraEnt = createEntity();
	Transform* cameraTransform = addComponentToEntity<Transform>(cameraEnt);
	cameraTransform->move(XMFLOAT3(0.0f, 0.0f, -10.0f));

	Camera* camera = addComponentToEntity<Camera>(cameraEnt);
	setMainCamera(camera);

	FreeCamControls* freeCamControls = addComponentToEntity<FreeCamControls>(cameraEnt);

	unsigned int directionalLightEnt = createEntity();
	Transform* directionalLightTransform = addComponentToEntity<Transform>(directionalLightEnt);
	directionalLightTransform->rotateLocal(XMFLOAT3(45.0f, 0.0f, 0.0f));

	LightComponent* directionalLight = addComponentToEntity<LightComponent>(directionalLightEnt);
	directionalLight->setLightType(LightType::DIRECTIONAL_LIGHT);

	LightSettings directionalLightSettings = directionalLight->getLightSettings();
	directionalLightSettings.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	directionalLight->setLightSettings(directionalLightSettings);

	unsigned int spotLightEnt = createEntity();
	Transform* spotLightTransform = addComponentToEntity<Transform>(spotLightEnt);
	spotLightTransform->moveLocalZ(10.0f);
	spotLightTransform->rotateLocalY(180.0f);

	LightComponent* spotLight = addComponentToEntity<LightComponent>(spotLightEnt);
	spotLight->setLightType(LightType::SPOT_LIGHT);

	LightSettings spotLightSettings = spotLight->getLightSettings();
	spotLightSettings.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	spotLightSettings.radius = 10.0f;
	spotLightSettings.spotAngle = 30.0f;
	spotLight->setLightSettings(spotLightSettings);

	unsigned int pointLightEnt = createEntity();
	Transform* pointLightTransform = addComponentToEntity<Transform>(pointLightEnt);

	LightComponent* pointLight = addComponentToEntity<LightComponent>(pointLightEnt);
	pointLight->setLightType(LightType::POINT_LIGHT);

	LightSettings pointLightSettings = pointLight->getLightSettings();
	pointLightSettings.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pointLightSettings.radius = 5.0f;
	pointLight->setLightSettings(pointLightSettings);

	unsigned int cubeEnt = createEntity();
	Transform* cubeTransform = addComponentToEntity<Transform>(cubeEnt);
	cubeTransform->moveZ(3.0f);
	//cubeTransform->rotateLocalY(45.0f);
	cubeTransform->scale(XMFLOAT3(9.0f, 9.0f, 0.0f));

	MeshRenderComponent* cubeMeshRenderComponent = addComponentToEntity<MeshRenderComponent>(cubeEnt);
	cubeMeshRenderComponent->setMaterial(cracksMaterial);
	cubeMeshRenderComponent->setMesh(cube);
	
	unsigned int torusEnt = createEntity();
	Transform* torusTransform = addComponentToEntity<Transform>(torusEnt);
	torusTransform->moveX(5.0f);

	MeshRenderComponent* torusMeshRenderComponent = addComponentToEntity<MeshRenderComponent>(torusEnt);
	torusMeshRenderComponent->setMesh(torus);
	torusMeshRenderComponent->setRenderStyle(RenderStyle::WIREFRAME);
	
	unsigned int coneEnt = createEntity();
	Transform* coneTransform = addComponentToEntity<Transform>(coneEnt);
	coneTransform->moveX(-5.0f);
	coneTransform->scaleY(4.0f);

	MeshRenderComponent* coneMeshRenderComponent = addComponentToEntity<MeshRenderComponent>(coneEnt);
	coneMeshRenderComponent->setMesh(cone);
	coneMeshRenderComponent->setRenderStyle(RenderStyle::SOLID_WIREFRAME);

	unsigned int guiEntity = createEntity();
	GUITransform* guiTransform = addComponentToEntity<GUITransform>(guiEntity);
	guiTransform->setPosition(XMFLOAT2(640, 360));
	GUIComponent* gui = addComponentToEntity<GUIComponent>(guiEntity);

	return true;
}
