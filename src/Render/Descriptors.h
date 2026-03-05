#pragma once
class Descriptors
{
public:
	Descriptors() = default;
	bool InitRTV(ID3D12Device* _device);
	bool InitDSV(ID3D12Device* _device);
	bool InitCBV(ID3D12Device* _device);
	ID3D12DescriptorHeap* GrtvHeap() { return m_rtvHeap; }
	ID3D12DescriptorHeap* GdsvHeap() { return m_dsvHeap; }
	ID3D12DescriptorHeap* GcbvHeap() { return m_cbvheap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GdsvHandle() const { return m_dsvHeap->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GrtvHandle() const { return m_rtvHeap->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GcbvHandle() const { return m_cbvheap->GetGPUDescriptorHandleForHeapStart(); }
	UINT GCrtvSize() { return m_rtvDescriptorSize; }
private:
	ID3D12DescriptorHeap* m_rtvHeap = nullptr;
	UINT m_rtvDescriptorSize = -1;
	ID3D12DescriptorHeap* m_dsvHeap = nullptr;
	UINT m_dsvDescriptorSize = -1;
	ID3D12DescriptorHeap* m_cbvheap = nullptr;

};

