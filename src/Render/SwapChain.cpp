#include "pch.h"
#include "SwapChain.h"

SwapChain::SwapChain()
{
}

bool SwapChain::Init(int _width, int _height, HWND _handle, IDXGIFactory4* _factory, ID3D12CommandQueue* _queue)
{
	IDXGISwapChain1* tempSwapChain = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Width = _width;
	swapChainDesc.Height = _height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_format = swapChainDesc.Format;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = _factory->CreateSwapChainForHwnd(_queue, _handle, &swapChainDesc,nullptr, nullptr, &tempSwapChain);
	if (FAILED(hr))
	{
		Log(true, "Failed to create swapchain for hwnd");
		return 1;
	}

	IDXGISwapChain3* swapchainref = nullptr;
	hr = tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapchainref)); // because i need acces to GetCurrentBackBufferIndex() so i cast to newer version
	if (FAILED(hr))
	{
		Log(true, "Failed to query swapchain for IDXGISwapChain3 interface");
		return 1;
	}

	m_swapChain = swapchainref;
	tempSwapChain->Release();

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<float>(_width);
	m_viewport.Height = static_cast<float>(_height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = _width;
	m_scissorRect.bottom = _height;
	return 0;
}