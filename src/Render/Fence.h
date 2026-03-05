#pragma once
class Fence
{
public:
	Fence() = default;
	bool Init(ID3D12Device* _device);
	~Fence();
	//void Signal(ID3D12CommandQueue* queue);
	void WaitForCompletion(ID3D12CommandQueue* _queue);
	ID3D12Fence* GFence() const { return m_fence; }
	UINT64 GCFenceValue() const { return m_fenceValue; }
	void SFenceValue(UINT64 _value) { m_fenceValue = _value; }
	HANDLE GFenceEvent() const { return m_fenceEvent; }
private:
	ID3D12Fence* m_fence = nullptr;
	UINT64 m_fenceValue = -1;
	HANDLE m_fenceEvent = nullptr;
};

