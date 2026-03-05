#include "pch.h"
#include "Fence.h"

bool Fence::Init(ID3D12Device* _device)
{
	m_fenceValue = 1; // Start at 1
	HRESULT hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(hr))
	{
		Log(true, "Failed to create fence");
		return 1;
	}
	m_fenceValue = 0;
	// Create an event handle to use for waiting
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	return 0;
}

Fence::~Fence()
{
}

//void Fence::Signal(ID3D12CommandQueue* queue)
//{
//}

void Fence::WaitForCompletion(ID3D12CommandQueue* _queue)
{
	const UINT64 fenceToWait = m_fenceValue;
	_queue->Signal(m_fence, fenceToWait);
	m_fenceValue++;

	if (m_fence->GetCompletedValue() < fenceToWait)
	{
		m_fence->SetEventOnCompletion(fenceToWait, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}
