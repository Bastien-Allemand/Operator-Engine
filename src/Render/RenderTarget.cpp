#include "pch.h"
#include "RenderTarget.h"

RenderTarget::~RenderTarget()
{
	for (int i = 0; i < m_bufferCount; i++)
	{
		m_backbuffer[i] = nullptr;
	}
	m_depthStencilBuffer = nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GCurrentBackBufferView(ID3D12DescriptorHeap* _rtvheap, UINT _rtvsize) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvheap->GetCPUDescriptorHandleForHeapStart(), m_currentBuffer, _rtvsize);
}
