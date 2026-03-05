#include "pch.h"
#include "PipelineStateObject.h"
#include "Utils.h"
#include <iostream>
#include <d3dx12.h>

bool PipelineStateObject::Init(ID3D12Device* _device)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	// Input Layout,Vertex Shader, Pixel Shader
	InitRootSig(_device);
	InitInputLayout(&psoDesc);
	InitVs(&psoDesc, L"../../res/Shaders/DefaultShader.hlsl");
	InitPs(&psoDesc, L"../../res/Shaders/DefaultShader.hlsl");

	psoDesc.pRootSignature = m_rootSig;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX; // Standard
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1; // No MSAA

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	HRESULT hr = _device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr))
	{
		Log(true, "Failed to create graphics pipeline state");
		return 1;
	}

	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC uiBlendDesc = {};
	uiBlendDesc.BlendEnable = TRUE;
	uiBlendDesc.LogicOpEnable = FALSE;
	uiBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	uiBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	uiBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	uiBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	uiBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	uiBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	uiBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	uiBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.BlendState.RenderTarget[0] = uiBlendDesc;

	hr = _device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateUI));
	if (FAILED(hr))
	{
		Log(true, "Failed to create UI graphics pipeline state");
		return 1;
	}

	// --- Compute PSOs pour les particules ---
	InitComputeRootSig(_device);

	ID3DBlob* csUpdate = Utils::CompileShader(L"../../res/Shaders/ParticleCompute.hlsl", nullptr, "CSUpdate", "cs_5_0");
	ID3DBlob* csEmit = Utils::CompileShader(L"../../res/Shaders/ParticleCompute.hlsl", nullptr, "CSEmit", "cs_5_0");

	D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
	computeDesc.pRootSignature = m_computeRootSig;

	computeDesc.CS = { csUpdate->GetBufferPointer(), csUpdate->GetBufferSize() };
	hr = _device->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&m_psoParticleUpdate));
	if (FAILED(hr))
	{
		Log(true, "Failed to create particle update compute PSO");
		return 1;
	}

	computeDesc.CS = { csEmit->GetBufferPointer(), csEmit->GetBufferSize() };
	hr = _device->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&m_psoParticleEmit));
	if (FAILED(hr))
	{
		Log(true, "Failed to create particle emit compute PSO");
		return 1;
	}

	// --- PSO de rendu des particules (billboard + blending) ---
	InitParticleDrawRootSig(_device);

	ID3DBlob* vsParticle = Utils::CompileShader(L"../../res/Shaders/ParticleShader.hlsl", nullptr, "VS", "vs_5_0");
	ID3DBlob* psParticle = Utils::CompileShader(L"../../res/Shaders/ParticleShader.hlsl", nullptr, "PS", "ps_5_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC particleDrawDesc = {};
	particleDrawDesc.pRootSignature = m_particleDrawRootSig;
	particleDrawDesc.VS = { vsParticle->GetBufferPointer(), vsParticle->GetBufferSize() };
	particleDrawDesc.PS = { psParticle->GetBufferPointer(), psParticle->GetBufferSize() };
	particleDrawDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	particleDrawDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // Billboard double face
	particleDrawDesc.SampleMask = UINT_MAX;
	particleDrawDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	particleDrawDesc.NumRenderTargets = 1;
	particleDrawDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	particleDrawDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	particleDrawDesc.SampleDesc.Count = 1;
	// Pas d'input layout : le VS utilise SV_VertexID / SV_InstanceID
	particleDrawDesc.InputLayout = { nullptr, 0 };

	// Blending additif + depth read sans write
	particleDrawDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	particleDrawDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC particleBlend = {};
	particleBlend.BlendEnable = TRUE;
	particleBlend.LogicOpEnable = FALSE;
	particleBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	particleBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	particleBlend.BlendOp = D3D12_BLEND_OP_ADD;
	particleBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
	particleBlend.DestBlendAlpha = D3D12_BLEND_ZERO;
	particleBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	particleBlend.LogicOp = D3D12_LOGIC_OP_NOOP;
	particleBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	particleDrawDesc.BlendState.RenderTarget[0] = particleBlend;

	hr = _device->CreateGraphicsPipelineState(&particleDrawDesc, IID_PPV_ARGS(&m_psoParticleDraw));
	if (FAILED(hr))
	{
		Log(true, "Failed to create particle draw PSO");
		return 1;
	}

	InitPostProcessRootSig(_device);

	return 0;
}

bool PipelineStateObject::LoadPostProcessShader(ID3D12Device* _device, const wchar_t* _shaderPath)
{
	ID3DBlob* cs = Utils::CompileShader(_shaderPath, nullptr, "CSMain", "cs_5_0");

	D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
	computeDesc.pRootSignature = m_postProcessRootSig;
	computeDesc.CS = { cs->GetBufferPointer(), cs->GetBufferSize() };
	/*m_psoPostProcess[_shaderPath] = nullptr;*/
	HRESULT hr = _device->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&m_psoPostProcess[_shaderPath]));
	if (FAILED(hr))
	{
		_Log((char*)cs->GetBufferPointer());
		cs->Release();
		Log(true, "Failed to create post-process compute PSO");		
		return 1;
	}
	return 0;
}

bool PipelineStateObject::InitComputeRootSig(ID3D12Device* _device)
{
	// Slot 0 : CBV b0 (CbParticle)
	// Slot 1 : UAV table u0 (particleBuffer)
	// Slot 2 : UAV table u1 (freeList avec counter)
	CD3DX12_DESCRIPTOR_RANGE uavTableParticles;
	uavTableParticles.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0

	CD3DX12_DESCRIPTOR_RANGE uavTableFreeList;
	uavTableFreeList.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1); // u1

	CD3DX12_ROOT_PARAMETER params[3];
	params[0].InitAsConstantBufferView(0); // b0
	params[1].InitAsDescriptorTable(1, &uavTableParticles);
	params[2].InitAsDescriptorTable(1, &uavTableFreeList);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(3, params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ID3DBlob* serialized = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
	if (FAILED(hr))
	{
		if (error) { std::cout << "Error: " << (char*)error->GetBufferPointer() << std::endl; error->Release(); }
		return 1;
	}
	hr = _device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_computeRootSig));
	if (FAILED(hr))
	{
		Log(true, "Failed to create compute root signature");
		return 1;
	}
	Log(false, "Initializing compute root signature");
	return 0;
}

bool PipelineStateObject::InitParticleDrawRootSig(ID3D12Device* _device)
{
	// Slot 0 : CBV b0 (CbPass — viewProj + eyePos)
	// Slot 1 : SRV table t0 (StructuredBuffer<Particle>)
	// Slot 2 : SRV table t1 (Texture2D — sprite)
	CD3DX12_DESCRIPTOR_RANGE srvParticles;
	srvParticles.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

	CD3DX12_DESCRIPTOR_RANGE srvTexture;
	srvTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1

	CD3DX12_ROOT_PARAMETER params[3];
	params[0].InitAsConstantBufferView(0); // b0
	params[1].InitAsDescriptorTable(1, &srvParticles, D3D12_SHADER_VISIBILITY_VERTEX);
	params[2].InitAsDescriptorTable(1, &srvTexture, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(3, params, 1, &linearWrap, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ID3DBlob* serialized = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
	if (FAILED(hr))
	{
		if (error) { std::cout << "Error: " << (char*)error->GetBufferPointer() << std::endl; error->Release(); }
		return 1;
	}
	hr = _device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_particleDrawRootSig));
	if (FAILED(hr))
	{
		Log(true, "Failed to create particle draw root signature");
		return 1;
	}
	Log(false, "Initializing particle root signature");
	return 0;
}

bool PipelineStateObject::InitPostProcessRootSig(ID3D12Device* _device)
{
	CD3DX12_DESCRIPTOR_RANGE uavOutput;
	uavOutput.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0

	CD3DX12_DESCRIPTOR_RANGE srvScene;
	srvScene.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

	CD3DX12_ROOT_PARAMETER params[3];
	params[0].InitAsConstantBufferView(0); // b0
	params[1].InitAsDescriptorTable(1, &srvScene, D3D12_SHADER_VISIBILITY_ALL);
	params[2].InitAsDescriptorTable(1, &uavOutput, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		0,								  // register s0
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(3, params, 1, &linearClamp, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ID3DBlob* serialized = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
	if (FAILED(hr))
	{
		if (error) { std::cout << "Error: " << (char*)error->GetBufferPointer() << std::endl; error->Release(); }
		return 1;
	}
	hr = _device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_postProcessRootSig));

	Log(FAILED(hr), "Initializing post-process root signature");
	return !FAILED(hr);
}

bool PipelineStateObject::InitRootSig(ID3D12Device* _device)
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_ROOT_PARAMETER rootParameters[3];
	rootParameters[0].InitAsConstantBufferView(0); //register b0
	rootParameters[1].InitAsConstantBufferView(1); //register b1

	rootParameters[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		0,                                // register s0
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,  
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(
		_countof(rootParameters),
		rootParameters,
		1,              // Nombre de samplers statiques
		&linearWrap,    // Pointeur vers le sampler
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);


	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);

	if (FAILED(hr))
	{
		Log(true, "Failed to serialize root signature");
		if (errorBlob)
		{
			std::cout << "Error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
		}
		return 1;
	}
	hr = _device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_rootSig));
	if (FAILED(hr))
	{
		Log(true, "Failed to create root signature");
		return 1;
	}
	return 0;
}

bool PipelineStateObject::InitVs(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc, const wchar_t* _filename)
{

	m_vertexShader = Utils::CompileShader(_filename, nullptr, "VS", "vs_5_0");

	if (m_vertexShader == nullptr)
	{
		Log(true, "Vertex shader compilation failed");
		return 1;
	}
	_desc->VS = { m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize() }; // if bug try casting to byte but it should work as is
	return 0;
}

bool PipelineStateObject::InitPs(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc, const wchar_t* _filename)
{
	m_pixelShader = Utils::CompileShader(_filename, nullptr, "PS", "ps_5_0");

	if (m_pixelShader == nullptr)
	{
		Log(true, "Pixel shader compilation failed");
		return 1;
	}
	_desc->PS = { m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize() }; // if bug try casting to byte but it should work as is
	return 0;
}

bool PipelineStateObject::InitInputLayout(D3D12_GRAPHICS_PIPELINE_STATE_DESC* _desc)
{
	m_inputLayout = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_desc->InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	return true;
}