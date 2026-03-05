#include "pch.h"
#include "ImageLoader.h"

ImageLoader::~ImageLoader()
{
    WaitForGPU();

    if (m_commandList) { m_commandList->Release(); m_commandList = nullptr; }
    if (m_commandAllocator) { m_commandAllocator->Release(); m_commandAllocator = nullptr; }
    if (m_fence) { m_fence->Release(); m_fence = nullptr; }
    if (m_fenceEvent) { CloseHandle(m_fenceEvent); m_fenceEvent = nullptr; }
}

void ImageLoader::Initialize(
    ID3D12Device* _device,
    ID3D12CommandQueue* _commandQueue,
    ID3D12DescriptorHeap* _srvHeap,
    UINT _descriptorSize)
{
    m_device = _device;
    m_commandQueue = _commandQueue;
    m_srvHeap = _srvHeap;
    m_descriptorSize = _descriptorSize;
    m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_commandAllocator)
    );

    m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator,
        nullptr,
        IID_PPV_ARGS(&m_commandList)
    );
    m_commandList->Close();
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    m_fenceValue = 1;
}

bool ImageLoader::LoadFromFile(const wchar_t* _filePath, Image& _outImage)
{
    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    HRESULT hr = DirectX::LoadDDSTextureFromFile(
        m_device,
        _filePath,
        &_outImage.texture,
        ddsData,
        subresources
    );

    if (FAILED(hr))
    {
        return false;
    }
    D3D12_RESOURCE_DESC desc = _outImage.texture->GetDesc();
    _outImage.width = static_cast<UINT>(desc.Width);
    _outImage.height = desc.Height;
    _outImage.format = desc.Format;
    _outImage.isLoaded = false;
    if (!UploadTexture(_outImage, subresources))
    {
        _outImage.texture->Release();
        _outImage.texture = nullptr;
        return false;
    }

    return true;
}

bool ImageLoader::UploadTexture(Image& _outImage, std::vector<D3D12_SUBRESOURCE_DATA>& _subresources)
{
    D3D12_RESOURCE_DESC desc = _outImage.texture->GetDesc();
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(
        _outImage.texture,
        0,
        static_cast<UINT>(_subresources.size())
    );
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    HRESULT hr = m_device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_outImage.uploadHeap)
    );

    if (FAILED(hr)) return false;
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator, nullptr);
    UpdateSubresources(
        m_commandList,
        _outImage.texture,
        _outImage.uploadHeap,
        0,
        0,
        static_cast<UINT>(_subresources.size()),
        _subresources.data()
    );
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _outImage.texture,
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->Close();
    ExecuteUploadCommands();
    if (!AllocateDescriptor(_outImage)) return false;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    m_device->CreateShaderResourceView(_outImage.texture, &srvDesc, _outImage.cpuHandle);
    _outImage.isLoaded = true;
    _outImage.uploadHeap->Release();
    _outImage.uploadHeap = nullptr;

    return true;
}

bool ImageLoader::AllocateDescriptor(Image& _outImage)
{
    _outImage.heapIndex = m_nextDescriptorIndex++;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
        m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
        _outImage.heapIndex,
        m_descriptorSize
    );

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(
        m_srvHeap->GetGPUDescriptorHandleForHeapStart(),
        _outImage.heapIndex,
        m_descriptorSize
    );

    _outImage.cpuHandle = cpuHandle;
    _outImage.gpuHandle = gpuHandle;

    return true;
}

void ImageLoader::ExecuteUploadCommands()
{
    ID3D12CommandList* commandLists[] = { m_commandList };
    m_commandQueue->ExecuteCommandLists(1, commandLists);
    const UINT64 fenceToWaitFor = m_fenceValue;
    m_commandQueue->Signal(m_fence, fenceToWaitFor);
    m_fenceValue++;
    if (m_fence->GetCompletedValue() < fenceToWaitFor)
    {
        m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void ImageLoader::BindForDraw(ID3D12GraphicsCommandList* _commandList, UINT _rootParameterIndex, const Image& _image)
{
    if (!_image.isLoaded) return;
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    _commandList->SetDescriptorHeaps(1, heaps);
    _commandList->SetGraphicsRootDescriptorTable(_rootParameterIndex, _image.gpuHandle);
}

void ImageLoader::WaitForGPU()
{
    if (!m_fence || !m_commandQueue) return;

    const UINT64 fenceToWaitFor = m_fenceValue;
    m_commandQueue->Signal(m_fence, fenceToWaitFor);
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fenceToWaitFor)
    {
        m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void ImageLoader::ReleaseImage(Image& _image)
{
    WaitForGPU();
    
    if (_image.texture) { _image.texture->Release(); _image.texture = nullptr; }
    if (_image.uploadHeap) { _image.uploadHeap->Release(); _image.uploadHeap = nullptr; }
    _image.isLoaded = false;
}
