#include "Scene1.h"

using namespace DirectX;

Scene1::Scene1(ID3D11Device* device, ID3D11DeviceContext* context) : Scene(device, context)
{
}

Scene1::~Scene1()
{
}

void Scene1::onLoad()
{
	//if (!loadFromJSON("Assets/Scenes/scene1.json")) return false;

	//// The onclick callback can't be saved to a json file, so set the callback here
	//Entity* guiButtonEntity = getEntityByName("Button");
	//GUIButtonComponent* guiButton = guiButtonEntity->getComponent<GUIButtonComponent>();
	//guiButton->setOnClickCallback([guiButton]()
	//{
	//	guiButton->setText("Yay!");
	//});

	//AssetManager::loadTexture("cracks", "cracks.png");
	//AssetManager::loadTexture("cracks_spec", "cracks_spec.png");
	//AssetManager::loadTexture("cracks_norm", "cracks_norm.png");
	//AssetManager::createMaterial("cracks", "cracks", "cracks_spec", "cracks_norm");
	//AssetManager::loadMesh("cube", "cube.obj");
	//AssetManager::loadMesh("torus", "torus.obj");
	//AssetManager::loadMesh("cone", "cone.obj");
	//AssetManager::loadFont("Arial_12pt", "Arial_12pt.spritefont");

	//Entity* cameraEnt = createEntity("Camera");
	//Transform* cameraTransform = cameraEnt->addComponent<Transform>();
	//cameraTransform->move(XMFLOAT3(0.0f, 0.0f, -10.0f));

	//CameraComponent* camera = cameraEnt->addComponent<CameraComponent>();
	//setMainCamera(camera);

	//cameraEnt->addComponent<FreeCamControls>();

	//Entity* directionalLightEnt = createEntity("Directional Light");
	//Transform* directionalLightTransform = directionalLightEnt->addComponent<Transform>();
	//directionalLightTransform->rotateLocal(XMFLOAT3(45.0f, 0.0f, 0.0f));

	//LightComponent* directionalLight = directionalLightEnt->addComponent<LightComponent>();
	//directionalLight->setLightType(LightType::DIRECTIONAL_LIGHT);

	//LightSettings directionalLightSettings = directionalLight->getLightSettings();
	//directionalLightSettings.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//directionalLight->setLightSettings(directionalLightSettings);

	//Entity* spotLightEnt = createEntity("Spot Light");
	//Transform* spotLightTransform = spotLightEnt->addComponent<Transform>();
	//spotLightTransform->moveLocalZ(10.0f);
	//spotLightTransform->rotateLocalY(180.0f);

	//LightComponent* spotLight = spotLightEnt->addComponent<LightComponent>();
	//spotLight->setLightType(LightType::SPOT_LIGHT);

	//LightSettings spotLightSettings = spotLight->getLightSettings();
	//spotLightSettings.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	//spotLightSettings.radius = 10.0f;
	//spotLightSettings.spotAngle = 30.0f;
	//spotLight->setLightSettings(spotLightSettings);

	//Entity* pointLightEnt = createEntity("Point Light");
	//Transform* pointLightTransform = pointLightEnt->addComponent<Transform>();

	//LightComponent* pointLight = pointLightEnt->addComponent<LightComponent>();
	//pointLight->setLightType(LightType::POINT_LIGHT);

	//LightSettings pointLightSettings = pointLight->getLightSettings();
	//pointLightSettings.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//pointLightSettings.radius = 5.0f;
	//pointLight->setLightSettings(pointLightSettings);

	//Entity* cubeEnt = createEntity("Cube");
	//Transform* cubeTransform = cubeEnt->addComponent<Transform>();
	//cubeTransform->moveZ(3.0f);
	//cubeTransform->scale(XMFLOAT3(9.0f, 9.0f, 0.0f));

	//MeshRenderComponent* cubeMeshRenderComponent = cubeEnt->addComponent<MeshRenderComponent>();
	//cubeMeshRenderComponent->setMaterial("cracks");
	//cubeMeshRenderComponent->setMesh("cube");
	//
	//Entity* torusEnt = createEntity("Torus");
	//Transform* torusTransform = torusEnt->addComponent<Transform>();
	//torusTransform->moveX(5.0f);

	//MeshRenderComponent* torusMeshRenderComponent = torusEnt->addComponent<MeshRenderComponent>();
	//torusMeshRenderComponent->setMesh("torus");
	//torusMeshRenderComponent->setRenderStyle(RenderStyle::WIREFRAME);
	//
	//Entity* coneEnt = createEntity("Cone");
	//Transform* coneTransform = coneEnt->addComponent<Transform>();
	//coneTransform->moveX(-5.0f);
	//coneTransform->scaleY(4.0f);

	//MeshRenderComponent* coneMeshRenderComponent = coneEnt->addComponent<MeshRenderComponent>();
	//coneMeshRenderComponent->setMesh("cone");
	//coneMeshRenderComponent->setRenderStyle(RenderStyle::SOLID_WIREFRAME);

	//Entity* guiSpriteEntity = createEntity("Sprite");
	//GUITransform* guiSpriteTransform = guiSpriteEntity->addComponent<GUITransform>();
	//guiSpriteTransform->setPosition(XMFLOAT2(640, 360));
	//guiSpriteTransform->setSize(XMFLOAT2(100.0f, 100.0f));
	//guiSpriteTransform->setOrigin(XMFLOAT2(0.5f, 0.5f));
	//GUISpriteComponent* sprite = guiSpriteEntity->addComponent<GUISpriteComponent>();

	//Entity* guiTextEntity = createEntity("Text");
	//GUITransform* guiTextTransform = guiTextEntity->addComponent<GUITransform>();
	//guiTextTransform->setPosition(XMFLOAT2(640, 360));
	//guiTextTransform->setOrigin(XMFLOAT2(0.5f, 0.5f));
	//GUITextComponent* textComponent = guiTextEntity->addComponent<GUITextComponent>();
	//textComponent->setColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	//std::string text = "HELLO";
	//textComponent->setText(text);
	//XMVECTOR originVector = textComponent->getFont()->MeasureString(std::wstring(text.begin(), text.end()).c_str()) / 2.0f;
	//XMFLOAT2 origin;
	//XMStoreFloat2(&origin, originVector);
	//guiTextTransform->setOrigin(origin);

	//Entity* guiButtonEntity = createEntity("Button");
	//GUITransform* guiButtonTransform = guiButtonEntity->addComponent<GUITransform>();
	//guiButtonTransform->setPosition(XMFLOAT2(640.0f, 50.0f));
	//guiButtonTransform->setOrigin(XMFLOAT2(0.5f, 0.5f));
	//guiButtonTransform->setSize(XMFLOAT2(80.0f, 30.0f));
	//GUIButtonComponent* guiButton = guiButtonEntity->addComponent<GUIButtonComponent>();
	//guiButton->setFont("Arial_12pt");
	//guiButton->setText("Click me");
	//guiButton->setOnClickCallback([guiButton]()
	//{
	//	guiButton->setText("Yay!");
	//});

	//saveToJSON("Assets/Scenes/scene1.json");

	Entity* cone = getEntityByName("Cone");
	Entity* cube = getEntityByName("Cube");

	cone->setParent(cube);

	//std::vector<Transform*> transforms = getAllComponentsByType<Transform>();
}

void Scene1::update(float deltaTime, float totalTime)
{
	Scene::update(deltaTime, totalTime);

	/*Entity* cube = getEntityByName("Cube");
	if (cube)
	{
		Transform* cubeTransform = cube->getComponent<Transform>();

		if (cubeTransform)
			cubeTransform->rotateAroundPoint(XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT3(0.0f, 45.0f * deltaTime, 0.0f));
	}*/
}
