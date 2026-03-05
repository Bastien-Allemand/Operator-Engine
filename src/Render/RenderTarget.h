#pragma once

class Descriptor;

class RenderTarget
{
public:
	RenderTarget()
	{
		for (int i = 0; i < m_bufferCount; ++i)
		{
			m_backbuffer[i] = nullptr;
		}
		m_depthStencilBuffer = nullptr;
	};
	~RenderTarget();
	ID3D12Resource* GCurrentBackBuffer() const { return m_backbuffer[m_currentBuffer]; }
	ID3D12Resource* GCurrentBackBuffer(int _index) const { return m_backbuffer[_index]; }

	ID3D12Resource* GDepthStencilBuffer() const { return m_depthStencilBuffer; }
	void SDepthStencilBuffer(ID3D12Resource* _buffer) { m_depthStencilBuffer = _buffer; }

	void SwapBuffers() { m_currentBuffer = (m_currentBuffer + 1) % m_bufferCount; }
	static const uint32 GCBufferCount() { return m_bufferCount; }
	void SCurrentBackBuffer(bool _count,ID3D12Resource* _buffer) { m_backbuffer[_count] = _buffer; }
	void SCurrentBackBuffer(bool _count) { m_currentBuffer = _count; }
	D3D12_CPU_DESCRIPTOR_HANDLE GCurrentBackBufferView(ID3D12DescriptorHeap* _rtvheap, UINT _rtvsize) const;
private:
	static const uint32 m_bufferCount = 2;
	ID3D12Resource* m_backbuffer[m_bufferCount];
	ID3D12Resource* m_depthStencilBuffer;

	bool m_currentBuffer = 0;
};

