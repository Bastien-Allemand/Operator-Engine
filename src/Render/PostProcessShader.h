#pragma once

class PostProcessShader
{
	public:
	PostProcessShader();
	~PostProcessShader();
	void Initialize(ID3D12Device* device);
	void Release();
	ID3D12PipelineState* GetPipelineState() const { return m_pipelineState; }
	ID3D12RootSignature* GetRootSignature() const { return m_rootSignature; }

private:
	ID3D12PipelineState* m_pipelineState = nullptr;
	ID3D12RootSignature* m_rootSignature = nullptr;
};

