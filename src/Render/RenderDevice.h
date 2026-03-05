#pragma once
class Factory;

class RenderDevice
{
public:
	RenderDevice() = default;

	bool Init(Factory* _factory);

	ID3D12Device* GDevice() const { return m_device; }
private:
	ID3D12Device* m_device = nullptr;
};

