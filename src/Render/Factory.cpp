#include "pch.h"
#include "Factory.h"

Factory::Factory()
{
    m_factory = nullptr;

}

bool Factory::InitFactory()
{
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory))))
        return 0;
    return 1;
}

bool Factory::InitAdapter()
{
    for (UINT adapterIndex = 0; m_factory->EnumAdapters1(adapterIndex, &m_adapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        m_adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;
        if (SUCCEEDED(D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            break;
    }
    return m_adapter != nullptr;
}

Factory::~Factory()
{
	if (m_factory)
	    m_factory->Release();
    if (m_adapter)
	    m_adapter->Release();
}
