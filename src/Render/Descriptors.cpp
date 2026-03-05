#include "pch.h"
#include "Descriptors.h"


bool Descriptors::InitRTV(ID3D12Device* _device)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2; // One for each buffer
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = _device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	if (FAILED(hr))
	{
		Log(true, "Failed to create RTV descriptor heap");
		return 1;
	}
	hr = m_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (FAILED(hr))
	{
		Log(true, "Failed to get RTV descriptor size");
		return 1;
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	return 0;
}

bool Descriptors::InitDSV(ID3D12Device* _device)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2; // One for each buffer
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = _device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));
	if (FAILED(hr))
	{
		Log(true, "Failed to create DSV descriptor heap");
		return 1;
	}
	hr = m_dsvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (FAILED(hr))
	{
		Log(true, "Failed to get DSV descriptor size");
		return 1;
	}
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	return 0;
}

bool Descriptors::InitCBV(ID3D12Device* _device)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 256; // One for each buffer
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	HRESULT hr = _device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvheap));
	if (FAILED(hr))
	{
		Log(true, "Failed to create CBV descriptor heap"); 
		return 1;
	}
	//hr = m_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (FAILED(hr))
	{
		Log(true, "Failed to get CBV descriptor size"); 
		return 1;
	}
	return 0;
}
