#include "Scene.h"

using namespace DirectX;

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;

	m_renderer = nullptr;
	m_guiRenderer = nullptr;

	m_rasterizerState = nullptr;
	m_prevUseWireframe = false;

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
	m_near = 0;
	m_far = 0;

	m_entities = std::vector<unsigned int>();
	m_components = std::unordered_map<unsigned int, std::vector<Component*>>();
	m_entityCounter = 0;

	m_lights = std::vector<LightComponent*>();
	m_cameras = std::vector<Camera*>();
	m_mainCamera = nullptr;
}

Scene::~Scene()
{
	m_lights.clear();
	m_cameras.clear();

	while (m_entities.size() > 0)
	{
		deleteEntity(m_entities.back());
	}

	if (m_rasterizerState) m_rasterizerState->Release();

	delete m_guiRenderer;
	delete m_renderer;
}

bool Scene::init()
{
	m_renderer = new Renderer(m_context);
	if (!m_renderer->init()) return false;

	m_guiRenderer = new GUIRenderer(m_context);
	if (!m_guiRenderer->init()) return false;

	// Add a rasterizer state so that we can control whether wireframe is on or off.
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.DepthClipEnable = true;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	HRESULT hr = m_device->CreateRasterizerState(&rastDesc, &m_rasterizerState);
	if (FAILED(hr))
	{
		Output::Error("Failed to create rasterizer state.");
		return false;
	}

	m_context->RSSetState(m_rasterizerState);

	return true;
}

void Scene::update(float deltaTime, float totalTime)
{
	// Update all entities in the scene
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		std::vector<Component*>* components = getComponentsOfEntity(m_entities[i]);
		for (unsigned int j = 0; j < components->size(); j++)
		{
			components->at(j)->update(deltaTime, totalTime);
		}
	}

	// Preprocess each camera by updating it's view matrix
	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		m_cameras[i]->updateViewMatrix();
	}
}

void Scene::render()
{
	// Preprocess each light entity to get it's position and direction (for forward rendering).
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>(MAX_LIGHTS);

	// Only loop through the lights if there is a main camera, since we need its view matrix.
	if (m_mainCamera)
	{
		for (unsigned int i = 0; i < m_lights.size(); i++)
		{
			//  Don't use disabled components
			if (!m_lights[i]->enabled) continue;

			const LightSettings lightSettings = m_lights[i]->getLightSettings();

			// Get position, direction, and type of each light
			Transform* lightTransform = getComponentOfEntity<Transform>(m_lights[i]->getEntity());
			if (lightTransform)
			{
				XMFLOAT3 lightPosition = lightTransform->getPosition();
				XMFLOAT3 lightDirection = lightTransform->getForward();

				unsigned int lightType = (unsigned int)m_lights[i]->getLightType();

				// Creates the final memory-aligned struct that is sent to the GPU
				lightData[i] =
				{
					lightSettings.color,
					lightDirection,
					lightSettings.brightness,
					lightPosition,
					lightSettings.specularity,
					lightSettings.radius,
					lightSettings.spotAngle,
					m_lights[i]->enabled,
					lightType,
				};
			}
		}
	}

	CommonStates states(m_device);
	ID3D11BlendState* blendState = states.Opaque();
	ID3D11DepthStencilState* depthStencilState = states.DepthDefault();

	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	if (lightData.size() > 0)
		m_renderer->render(this, blendState, &lightData[0], lightData.size());
	else
		m_renderer->render(this, blendState, nullptr, 0);

	blendState = states.NonPremultiplied();
	depthStencilState = states.DepthRead();

	m_context->OMSetBlendState(blendState, nullptr, 0xffffffff);
	m_context->OMSetDepthStencilState(depthStencilState, 0);

	if (m_guis.size() > 0)
	{
		m_guiRenderer->render(this, blendState, depthStencilState, &m_guis[0], m_guis.size());
	}
}

DirectX::XMMATRIX Scene::getProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

void Scene::updateProjectionMatrix(int width, int height, float nearZ, float farZ)
{
	m_near = nearZ;
	m_far = farZ;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / height, m_near, m_far);
	XMStoreFloat4x4(&m_projectionMatrix, perspective);
}

float Scene::getNearZ() const
{
	return m_near;
}

float Scene::getFarZ() const
{
	return m_far;
}

Camera* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(Camera* camera)
{
	m_mainCamera = camera;
}

void Scene::drawInWireframeMode(bool wireframe)
{
	if (m_prevUseWireframe == wireframe) return;

	// Get the rasterizer state's current settings
	D3D11_RASTERIZER_DESC rasterizerDesc;
	m_rasterizerState->GetDesc(&rasterizerDesc);

	if (wireframe)
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.DepthBias = -1000;
		rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	}
	else
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	}

	// Recreate the entire rasterizer state
	m_rasterizerState->Release();
	m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	m_context->RSSetState(m_rasterizerState);

	m_prevUseWireframe = wireframe;
}

void Scene::subscribeMouseDown(Component* component)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		if (m_mouseDownCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseDownCallbacks.push_back(component);
}

void Scene::unsubscribeMouseDown(Component* component)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		if (m_mouseDownCallbacks[i] == component)
		{
			m_mouseDownCallbacks.erase(m_mouseDownCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseUp(Component* component)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		if (m_mouseUpCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseUpCallbacks.push_back(component);
}

void Scene::unsubscribeMouseUp(Component* component)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		if (m_mouseUpCallbacks[i] == component)
		{
			m_mouseUpCallbacks.erase(m_mouseUpCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseMove(Component* component)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		if (m_mouseMoveCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseMoveCallbacks.push_back(component);
}

void Scene::unsubscribeMouseMove(Component* component)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		if (m_mouseMoveCallbacks[i] == component)
		{
			m_mouseMoveCallbacks.erase(m_mouseMoveCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::subscribeMouseWheel(Component* component)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		if (m_mouseWheelCallbacks[i] == component)
		{
			Output::Warning("Component not subscribed because callback already exists.");
			return;
		}
	}

	m_mouseWheelCallbacks.push_back(component);
}

void Scene::unsubscribeMouseWheel(Component* component)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		if (m_mouseWheelCallbacks[i] == component)
		{
			m_mouseWheelCallbacks.erase(m_mouseWheelCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
}

void Scene::onMouseDown(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		m_mouseDownCallbacks[i]->onMouseDown(buttonState, x, y);
	}
}

void Scene::onMouseUp(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		m_mouseUpCallbacks[i]->onMouseUp(buttonState, x, y);
	}
}

void Scene::onMouseMove(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		m_mouseMoveCallbacks[i]->onMouseMove(buttonState, x, y);
	}
}

void Scene::onMouseWheel(float wheelDelta, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		m_mouseWheelCallbacks[i]->onMouseWheel(wheelDelta, x, y);
	}
}

unsigned int Scene::createEntity()
{
	unsigned int entity = ++m_entityCounter;

	m_entities.push_back(entity);
	m_components[entity] = std::vector<Component*>();

	return entity;
}

void Scene::deleteEntity(unsigned int entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			m_entities.erase(m_entities.begin() + i);

			for (unsigned int j = 0; j < m_components.size(); j++)
			{
				for (unsigned int k = 0; k < m_components[entity].size(); k++)
				{
					delete m_components[entity][k];
				}
				m_components.erase(entity);
			}
			break;
		}
	}
}

void Scene::getAllEntities(unsigned int** entities, unsigned int* entityCount)
{
	*entityCount = m_entities.size();

	if (*entityCount > 0)
		*entities = &m_entities[0];
	else
		*entities = nullptr;
}

void Scene::getAllLights(LightComponent*** lights, unsigned int* lightCount)
{
	*lightCount = m_lights.size();
	
	if (*lightCount > 0)
		*lights = &m_lights[0];
	else
		*lights = nullptr;
	
}

std::vector<Component*>* Scene::getComponentsOfEntity(unsigned int entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			return &m_components.at(entity);
		}
	}

	return nullptr;
}
