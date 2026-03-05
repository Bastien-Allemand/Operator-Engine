#pragma once
#include "Engine/IPostProcessSystemRessource.h"

class PostProcessResource : public IPostProcessSystemRessource
{
	public:
		CD3DX12_RESOURCE_DESC desc;
		ID3D12Resource* srvResource = nullptr;
		ID3D12Resource* uavResource = nullptr;

		D3D12_RESOURCE_STATES srvState = D3D12_RESOURCE_STATE_COMMON;
		D3D12_RESOURCE_STATES uavState = D3D12_RESOURCE_STATE_COMMON;

	struct DescriptorPair {
		D3D12_CPU_DESCRIPTOR_HANDLE cpu;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu;
	};

	DescriptorPair srvHandle;
	DescriptorPair uavHandle;

	void Release() override {
		if (srvResource) srvResource->Release();
		if (uavResource) uavResource->Release();
	}
};

