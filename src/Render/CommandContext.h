#pragma once
class RenderDevice;

class CommandContext 
{
public:
    CommandContext() = default;

    bool Init(RenderDevice* _renderDevice);

	ID3D12GraphicsCommandList* GCommandList() const { return m_list; }
	ID3D12CommandAllocator* GCommandAllocator() const { return m_allocators; }

    void CloseAndExecute(ID3D12CommandQueue* _queue);

private:
    ID3D12GraphicsCommandList* m_list = nullptr;
    ID3D12CommandAllocator* m_allocators = nullptr;

};

