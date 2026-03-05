#pragma once
template<typename T>
class ConstantBuffer
{
public:
    ConstantBuffer() = default;
    ~ConstantBuffer() {
        if (m_resource) m_resource->Unmap(0, nullptr);
    }

    bool Init(ID3D12Device* device, ID3D12DescriptorHeap* heap, UINT descriptorIndex);
    void CopyData(int elementindex,const T& data) {
        memcpy(&m_mappedData[elementindex*sizeof(T)], &data, sizeof(T));
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle() const { return m_gpuHandle; }
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const { return m_resource->GetGPUVirtualAddress(); }
private:
    ID3D12Resource* m_resource = nullptr;
    BYTE* m_mappedData = nullptr;
    UINT m_bufferSize = 0;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = {};
};

template<typename T>
inline bool ConstantBuffer<T>::Init(ID3D12Device* device, ID3D12DescriptorHeap* heap, UINT descriptorIndex)
{
    // Constant buffer size must be a multiple of 256 bytes
    m_bufferSize = (sizeof(T) + 255) & ~255;

    // 1. Create the Resource on an Upload Heap
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_resource));

    if (FAILED(hr)) return 1;

    // 2. Map the resource (keep it mapped for the lifetime of the object)
    m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData));

    // 3. Create the CBV in the provided heap
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_resource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = m_bufferSize;

    UINT handleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(heap->GetCPUDescriptorHandleForHeapStart(), descriptorIndex, handleSize);
    m_gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, handleSize);

    device->CreateConstantBufferView(&cbvDesc, cpuHandle);

    return 0;
}

