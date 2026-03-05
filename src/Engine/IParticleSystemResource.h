#pragma once

class IParticleSystemResource
{
	public:
	virtual ~IParticleSystemResource() = default;
	virtual void Release() = 0;
};