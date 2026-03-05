#pragma once
#include "IParticleSystemResource.h"

struct ParticleComponent
{
public:
    IParticleSystemResource* gpuResource = nullptr;

    bool isInitialized = false;

    ~ParticleComponent() {
        if (gpuResource) {
            gpuResource->Release();
            delete gpuResource;
            gpuResource = nullptr;
        }
    }
};