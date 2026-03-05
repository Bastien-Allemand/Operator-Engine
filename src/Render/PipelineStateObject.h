#pragma once

class PipelineStateObject
{
public:
	PipelineStateObject() = default;
	bool Init(ID3D12Device* _device);
	ID3D12PipelineState* GPipelineState() { return m_pipelineState; }
	ID3D12PipelineState* GPipelineStateUI() { return m_pipelineStateUI; }
	ID3D12PipelineState* GPsoParticleUpdate() { return m_psoParticleUpdate; }
	ID3D12PipelineState* GPsoParticleEmit() { return m_psoParticleEmit; }
	ID3D12PipelineState* GPsoParticleDraw() { return m_psoParticleDraw; }
	ID3D12PipelineState* GPsoPostProcess(WString _shaderPath) { return m_psoPostProcess[_shaderPath]; }
	ID3D12RootSignature* GRootSig() { return m_rootSig; }
	ID3D12RootSignature* GComputeRootSig() { return m_computeRootSig; }
	ID3D12RootSignature* GParticleDrawRootSig() { return m_particleDrawRootSig; }
	ID3D12RootSignature* GPostProcessRootSig() { return m_postProcessRootSig; }
	bool LoadPostProcessShader(ID3D12Device* _device, const wchar_t* _shaderPath);
private:
	ID3D12PipelineState* m_pipelineState = nullptr;
	ID3D12PipelineState* m_pipelineStateUI = nullptr;
	ID3D12PipelineState* m_psoParticleUpdate = nullptr;
	ID3D12PipelineState* m_psoParticleEmit = nullptr;
	ID3D12PipelineState* m_psoParticleDraw = nullptr;
	UnOrderedMap<WString, ID3D12PipelineState*> m_psoPostProcess;

	ID3D12RootSignature* m_rootSig = nullptr;
	ID3D12RootSignature* m_computeRootSig = nullptr;
	ID3D12RootSignature* m_particleDrawRootSig = nullptr;
	ID3D12RootSignature* m_postProcessRootSig = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
	ID3DBlob* m_vertexShader = nullptr;
	ID3DBlob* m_pixelShader = nullptr;

	bool InitRootSig(ID3D12Device* _device);
	bool InitComputeRootSig(ID3D12Device* _device);
	bool InitParticleDrawRootSig(ID3D12Device* _device);
	bool InitPostProcessRootSig(ID3D12Device* _device);
	bool InitVs(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc, const wchar_t* _filename);
	bool InitPs(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc, const wchar_t* _filename);
	bool InitInputLayout(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc);
};

