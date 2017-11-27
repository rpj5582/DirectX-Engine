#include "GUIRenderer.h"

using namespace DirectX;

GUIRenderer::GUIRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_spriteBatch = new SpriteBatch(m_context);

	m_blendState = nullptr;
	m_depthStencilStateRead = nullptr;
}

GUIRenderer::~GUIRenderer()
{
	m_device = nullptr;
	m_context = nullptr;

	delete m_spriteBatch;

	if (m_blendState) m_blendState->Release();
	if (m_depthStencilStateRead) m_depthStencilStateRead->Release();
}

bool GUIRenderer::init()
{
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_DEPTH_STENCIL_DESC depthStencilReadDesc = {};
	depthStencilReadDesc.DepthEnable = true;
	depthStencilReadDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilReadDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilReadDesc.StencilEnable = false;
	depthStencilReadDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilReadDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilReadDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilReadDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilReadDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilReadDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	HRESULT hr = m_device->CreateBlendState(&blendDesc, &m_blendState);
	if (FAILED(hr))
	{
		Debug::error("Failed to create blend state.");
		return false;
	}

	hr = m_device->CreateDepthStencilState(&depthStencilReadDesc, &m_depthStencilStateRead);
	if (FAILED(hr))
	{
		Debug::error("Failed to create read depth stencil state.");
		return false;
	}

	m_context->OMSetBlendState(m_blendState, nullptr, 0xffffffff);
	return true;
}

void GUIRenderer::begin()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_blendState, nullptr, m_depthStencilStateRead);
}

void GUIRenderer::render(const GUIComponent* const* guiComponents, size_t guiCount)
{
	for (unsigned int i = 0; i < guiCount; i++)
	{
		guiComponents[i]->draw(*m_spriteBatch);
	}
}

void GUIRenderer::end()
{
	m_spriteBatch->End();
}
