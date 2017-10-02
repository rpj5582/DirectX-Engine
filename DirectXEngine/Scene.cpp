#include "Scene.h"

using namespace DirectX;

Scene::Scene(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_rasterizerState = nullptr;

	m_renderStyle = RenderStyle::SOLID;
	m_prevUseWireframe = false;

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());

	m_renderer = nullptr;

	m_entities = std::vector<unsigned int>();
	m_components = std::unordered_map<unsigned int, std::vector<Component*>>();
	m_entityCounter = 0;

	m_lights = std::vector<LightComponent*>();
	m_cameras = std::vector<Camera*>();
	m_mainCamera = nullptr;
}

Scene::~Scene()
{
	delete m_renderer;

	m_lights.clear();
	m_cameras.clear();

	while (m_entities.size() > 0)
	{
		deleteEntity(m_entities.back());
	}

	if (m_rasterizerState) m_rasterizerState->Release();
}

bool Scene::init()
{
	// Added a rasterizer state so that we can control whether wireframe is on or off.
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

	m_renderer = new Renderer();
	return true;
}

void Scene::update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_F1) && 0x8000)
	{
		m_renderStyle = RenderStyle::SOLID;
	}

	if (GetAsyncKeyState(VK_F2) && 0x8000)
	{
		m_renderStyle = RenderStyle::WIREFRAME;
	}

	if (GetAsyncKeyState(VK_F3) && 0x8000)
	{
		m_renderStyle = RenderStyle::SOLID_WIREFRAME;
	}

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		std::vector<Component*>* components = getComponentsOfEntity(m_entities[i]);
		for (unsigned int j = 0; j < components->size(); j++)
		{
			components->at(j)->update(deltaTime, totalTime);
		}
	}
}

void Scene::draw()
{
	// Don't continue if there is no main camera
	if (!m_mainCamera) return; 

	// Preprocess each light entity to get it's position and direction
	std::vector<GPU_LIGHT_DATA> lightData = std::vector<GPU_LIGHT_DATA>();

	for (unsigned int i = 0; i < m_lights.size(); i++)
	{
		const LightSettings lightSettings = m_lights[i]->getLightSettings();

		// Get position, direction, and type of each light
		Transform* lightTransform = getComponentOfEntity<Transform>(m_lights[i]->getEntity());
		if (lightTransform)
		{
			XMFLOAT3 lightPosition = lightTransform->getPosition();
			XMFLOAT3 lightDirection = lightTransform->getForward();
			unsigned int lightType = (unsigned int)m_lights[i]->getLightType();

			// Creates the final memory-aligned struct that is sent to the GPU
			lightData.push_back(
			{
				lightSettings.color,
				lightDirection,
				lightSettings.brightness,
				lightPosition,
				lightSettings.specularity,
				lightSettings.radius,
				lightSettings.spotAngle,
				lightSettings.spotFalloff,
				lightType
			});
		}
	}

	// Preprocess each camera by updating it's view matrix
	for (unsigned int i = 0; i < m_cameras.size(); i++)
	{
		m_cameras[i]->updateViewMatrix();
	}

	switch (m_renderStyle)
	{
	case RenderStyle::SOLID:
		useWireframe(false);
		m_renderer->render(this, m_context, &lightData[0], lightData.size());
		break;

	case RenderStyle::WIREFRAME:
		useWireframe(true);
		m_renderer->render(this, m_context, nullptr, 0);
		break;

	case RenderStyle::SOLID_WIREFRAME:
		useWireframe(false);
		m_renderer->render(this, m_context, &lightData[0], lightData.size());
		useWireframe(true);
		m_renderer->render(this, m_context, nullptr, 0);
		break;

	default:
		break;
	}	
}

DirectX::XMMATRIX Scene::getProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

void Scene::updateProjectionMatrix(int width, int height)
{
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / height, 0.1f, 100.0f);
	XMStoreFloat4x4(&m_projectionMatrix, perspective);
}

Camera* Scene::getMainCamera() const
{
	return m_mainCamera;
}

void Scene::setMainCamera(Camera* camera)
{
	m_mainCamera = camera;
}

void Scene::useWireframe(bool wireframe)
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
	*entities = &m_entities[0];
	*entityCount = m_entities.size();
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
