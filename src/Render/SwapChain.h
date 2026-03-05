#pragma once
class SwapChain
{
public:
	SwapChain();
	bool Init(int _width, int _height, HWND _handle, IDXGIFactory4* _factory, ID3D12CommandQueue* _queue);

	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	DXGI_FORMAT GFormat() const { return m_format; }

	IDXGISwapChain3* GSwapChain() const { return m_swapChain; }
	D3D12_VIEWPORT* GViewport() { return &m_viewport; }
	D3D12_RECT* GScissorRect() { return &m_scissorRect; }


private:
	IDXGISwapChain3* m_swapChain = nullptr;
	DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
};

