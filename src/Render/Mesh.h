#pragma once
#include "Geometry.h"
#include <DirectXCollision.h>
#include <d3d12.h>
class CommandContext;
class RenderDevice;

class Mesh
{
public:
    size_t GetIndexCount() const;
	DirectX::BoundingSphere GBounds() const { return geoBounds; }
	void BuildGeometry(const Geometry& _geo);
    bool Upload(ID3D12Device* _rd, ID3D12GraphicsCommandList* _list);
    void Bind(ID3D12GraphicsCommandList* _list);
private:
    bool geoSet = false;

	DirectX::BoundingSphere geoBounds;  

    uint32 m_vertexCount = 0;
    Vector<Vertex> m_vertices;

    uint32 m_indexCount = 0;
    Vector<uint32> m_indices;

    ID3DBlob* m_vBuffer = nullptr;
    ID3DBlob* m_iBuffer = nullptr;
    ID3D12Resource* m_vGPUBuffer = nullptr;
    ID3D12Resource* m_iGPUBuffer = nullptr;
	ID3D12Resource* m_vUpload = nullptr;
	ID3D12Resource* m_iUpload = nullptr;

    UINT m_vertexByteStride = 0;
    UINT m_vertexBufferByteSize = 0;
    DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;
    UINT m_indexBufferByteSize = 0;

	void UploadIndex(ID3D12Device* _rd, ID3D12GraphicsCommandList* _list);
	void UploadVertex(ID3D12Device* _rd, ID3D12GraphicsCommandList* _list);
	ID3D12Resource* CreateDefaultBuffer(ID3D12Device* _rd, ID3D12GraphicsCommandList* _list, const void* _data, UINT64 _byteSize, ID3D12Resource* _uploadBuffer);
	bool m_isUploaded = false;
};