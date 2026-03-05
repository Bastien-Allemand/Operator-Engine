#pragma once
#include "Engine/IParticleSystemResource.h"

class ParticleSystemResource : public IParticleSystemResource
{
public:
    ID3D12Resource* particleBuffer = nullptr; 
    ID3D12Resource* freeListBuffer = nullptr; 
    ID3D12Resource* counterBuffer = nullptr; 

    uint32_t maxParticles = 0;
    uint32_t aliveCount = 0;

    struct DescriptorPair 
    {
        DescriptorPair() : cpu({}), gpu({}) {};

        D3D12_CPU_DESCRIPTOR_HANDLE cpu;
        D3D12_GPU_DESCRIPTOR_HANDLE gpu;
    };

    DescriptorPair uavParticles;
    DescriptorPair uavFreeList; 
    DescriptorPair srvParticles;

    ParticleSystemResource() : uavParticles(), uavFreeList(), srvParticles() {};

    void Release() override {
        if (particleBuffer) particleBuffer->Release();
        if (freeListBuffer) freeListBuffer->Release();
        if (counterBuffer)  counterBuffer->Release();
    }
};

