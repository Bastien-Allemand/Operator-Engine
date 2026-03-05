#include "pch.h"
#include "RenderEngine.h"
#include "d3dx12.h"

#include "Mesh.h"
#include "Fence.h"
#include "Factory.h"

//#include "../Engine/Geometry.h"

#include "Geometry.h"
#include "Camera.h"	
#include "SwapChain.h"
#include "Descriptors.h"
#include "RenderDevice.h"
#include "RenderTarget.h"
#include "DirectXColors.h"
#include "CommandContext.h"
#include "Core/Transform.h"
#include "ConstantBuffer.h"
#include "PipelineStateObject.h"
#include "ImageLoader.h"
#include "Core/Texture.h"
#include "Core/Particle.h"
#include "ParticleSystemResource.h"
#include "PostProcessResource.h"

RenderEngine::~RenderEngine()
{
	if (m_renderDevice && m_renderDevice->GDevice())
	{
		FlushCommandQueue();
	}

	if (m_queue) m_queue->Release();

	delete m_factory;
	delete m_renderDevice;
	delete m_commandContext;
	delete m_pipelineStateObject;
	delete m_swapChain;
	delete m_fence;
	delete m_desc;
	delete m_renderTarget;
	delete m_passCB;
	delete m_passUICB;
	delete m_imageLoader;
	delete m_particleCB;
	delete m_postProcessCB;
}

bool RenderEngine::Init(int _width, int _height, HWND _handle)
{
	HardInit();
	bool r;
	//init factory
	r = m_factory->InitFactory(); Log(r, "Initializing Factory")
	r = m_renderDevice->Init(m_factory); Log(r, "Initializing Render Device")
	r = m_commandContext->Init(m_renderDevice);	Log(r, "Initializing CommandContext")
	r = InitQueue(); Log(r, "Initializing Command Queue")
	r = m_swapChain->Init(_width, _height, _handle, m_factory->GFactory(), m_queue); Log(r, "Initializing SwapChain")
	r = m_desc->InitRTV(m_renderDevice->GDevice()); Log(r, "Initializing RTV")
	r = m_desc->InitDSV(m_renderDevice->GDevice()); Log(r, "Initializing DSV")
	r = m_desc->InitCBV(m_renderDevice->GDevice()); Log(r, "Initializing CBV")
	uint32 descriptorSize = m_renderDevice->GDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_imageLoader->Initialize(m_renderDevice->GDevice(), m_queue, m_desc->GcbvHeap(), descriptorSize);
	r = m_pipelineStateObject->Init(m_renderDevice->GDevice()); Log(r, "Initializing PSO")
	r = m_fence->Init(m_renderDevice->GDevice()); Log(r, "Initializing Fence")
	r = Resize(_width, _height); Log(r,"Resizing")
	r = m_passCB->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 1); Log(r, "Initializing Light Constant Buffer")
	r = m_passUICB->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 2); Log(r, "Initializing UI Pass Constant Buffer")
	r = m_particleCB->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 3); Log(r, "Initializing Particle Constant Buffer")
	r = m_postProcessCB->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 4); Log(r, "Initializing Post Process Constant Buffer")

	Geometry quad;
	Vertex* vertices = new Vertex[4];
	vertices[0] = { {0,0,0}, {0,0,-1}, {0,0}, {1,1,1,1} };
	vertices[1] = { {1,0,0}, {0,0,-1}, {1,0}, {1,1,1,1} };
	vertices[2] = { {0,1,0}, {0,0,-1}, {0,1}, {1,1,1,1} };
	vertices[3] = { {1,1,0}, {0,0,-1}, {1,1}, {1,1,1,1} };
	quad.vertices = vertices;
	quad.vertexCount = 4;
	uint32* indices = new uint32[6];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;
	quad.indices = indices;
	quad.indexCount = 6;
	
	m_quadMesh = new Mesh();
	InitMesh(*m_quadMesh, quad);	

	return 0;
}

void RenderEngine::Update(float _dt)
{
	CbPass cb;

	cb.pointLight = *m_pointLight;
	cb.spotLight = *m_spotLight;
	cb.ambientLight = *m_ambientLight;
	cb.dirLight = *m_dirLight;

	Matrix view = BuildViewMatrix();
	Matrix Proj = BuildProjMatrix();

	Matrix viewProj = view * Proj;
	DirectX::XMStoreFloat4x4(&cb.gViewProj, DirectX::XMMatrixTranspose(viewProj));
	cb.eyePosW = m_cameraTransform->position;

	m_passCB->CopyData(0, cb);
}

IParticleSystemResource* RenderEngine::CreateParticleSystem(uint32_t _maxParticles) {
	ParticleSystemResource* res = new ParticleSystemResource();
	res->maxParticles = _maxParticles;

	UINT particleStride = sizeof(Particle);
	CreateStructuredBuffer(_maxParticles, particleStride, &res->particleBuffer, true);

	// Mettre tous les lifeTime à -1 pour que le shader les ignore
	{
		UINT64 bufSize = (UINT64)_maxParticles * particleStride;
		ID3D12Resource* uploadZero = nullptr;
		auto uploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);
		m_renderDevice->GDevice()->CreateCommittedResource(
			&uploadProp, D3D12_HEAP_FLAG_NONE, &bufDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadZero));

		Particle* pData = nullptr;
		uploadZero->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		for (uint32_t i = 0; i < _maxParticles; ++i)
		{
			memset(&pData[i], 0, particleStride);
			pData[i].lifeTime = -1.0f;  // Marque comme mort
		}
		uploadZero->Unmap(0, nullptr);

		auto list = m_commandContext->GCommandList();
		m_commandContext->GCommandAllocator()->Reset();
		list->Reset(m_commandContext->GCommandAllocator(), nullptr);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(res->particleBuffer,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		list->ResourceBarrier(1, &barrier);
		list->CopyBufferRegion(res->particleBuffer, 0, uploadZero, 0, bufSize);
		auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(res->particleBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		list->ResourceBarrier(1, &barrier2);

		m_commandContext->CloseAndExecute(m_queue);
		FlushCommandQueue();
		uploadZero->Release();
	}

	CreateStructuredBuffer(_maxParticles, sizeof(uint32_t), &res->freeListBuffer, true);

	CreateCounterBuffer(&res->counterBuffer);

	InitializeFreeList(res, _maxParticles);

	CreateParticleUAVs(res, _maxParticles);
	CreateParticleSRV(res, _maxParticles);

	return res;
}

float RenderEngine::GCAspectRatio() const
{
	return m_swapChain->GViewport()->Width / m_swapChain->GViewport()->Height;
}

Texture* RenderEngine::LoadTexture(const wchar_t* path)
{
	if (m_textureCache.find(path) != m_textureCache.end())
		return m_textureCache[path];
	return CreateTextureResource(path);
}

void RenderEngine::SLight(Light* _light,int _type)
{
	switch (_type)
	{
	case 0:
		m_ambientLight = _light;
		break;
	case 1:
		m_dirLight = _light;
		break;
	case 2:
		m_pointLight = _light;
		break;
	case 3:
		m_spotLight = _light;
		break;
	default:
		DebugMsg("Unknown light type", DebugFlag::WARNING);
		break;
	}
}

void RenderEngine::InitMesh(Mesh& _mesh,Geometry _geo)
{
	_mesh.BuildGeometry(_geo);
	m_commandContext->GCommandAllocator()->Reset();
	m_commandContext->GCommandList()->Reset(m_commandContext->GCommandAllocator(), nullptr);
	_mesh.Upload(m_renderDevice->GDevice(), m_commandContext->GCommandList());
	m_commandContext->CloseAndExecute(m_queue);
}

void RenderEngine::BeginDraw()
{
	auto list = m_commandContext->GCommandList();
	m_commandContext->GCommandAllocator()->Reset();
	list->Reset(m_commandContext->GCommandAllocator(), nullptr);

	list->RSSetViewports(1, m_swapChain->GViewport());
	list->RSSetScissorRects(1, m_swapChain->GScissorRect());

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	list->ResourceBarrier(1, &barrier);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderTarget->GCurrentBackBufferView(m_desc->GrtvHeap(), m_desc->GCrtvSize());

	list->ClearRenderTargetView(rtvHandle, DirectX::Colors::Black, 0, nullptr);
	list->ClearDepthStencilView(m_desc->GdsvHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	auto dsvHandle = m_desc->GdsvHandle();
	list->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_desc->GcbvHeap() };
	list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	list->SetGraphicsRootSignature(m_pipelineStateObject->GRootSig());

	list->SetPipelineState(m_pipelineStateObject->GPipelineState());

	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	list->SetGraphicsRootConstantBufferView(1, m_passCB->GetAddress());
}


void RenderEngine::Draw(Mesh* _mesh, Texture* _texture, Matrix _matrix)
{
	CbPerObject cb;

	Matrix world = DirectX::XMLoadFloat4x4(&m_world);

	_matrix = _matrix * world; 
	Matrix worldInvTrans = DirectX::XMMatrixInverse(nullptr, _matrix);
	DirectX::XMStoreFloat4x4(&cb.gWorldInvTrans, DirectX::XMMatrixTranspose(worldInvTrans));
	DirectX::XMStoreFloat4x4(&cb.gWorld, DirectX::XMMatrixTranspose(_matrix));
	cb.gColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (m_instanceCB.size() <= m_instanceCbIndex)
	{

		m_instanceCB.push_back(new ConstantBuffer<CbPerObject>());
		bool r = m_instanceCB[m_instanceCbIndex]->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 3 + m_instanceCbIndex);
		Log(r, "Initializing new Instance Constant Buffer");
	}

	m_instanceCB[m_instanceCbIndex]->CopyData(0, cb);

	auto list = m_commandContext->GCommandList();

	list->SetGraphicsRootConstantBufferView(0, m_instanceCB[m_instanceCbIndex]->GetAddress());

	if (_texture && _texture->image->texture != nullptr)
	{
		list->SetGraphicsRootDescriptorTable(2, _texture->image->gpuHandle);
	}

	_mesh->Bind(list);

	list->DrawIndexedInstanced((UINT)_mesh->GetIndexCount(), 1, 0, 0, 0);

	m_instanceCbIndex++;
}

void RenderEngine::CloseDraw()
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandContext->GCommandList()->ResourceBarrier(1, &barrier);

	m_commandContext->CloseAndExecute(m_queue);

	HRESULT hr = m_swapChain->GSwapChain()->Present(0, 0);
	if (FAILED(hr)) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			HRESULT reason = m_renderDevice->GDevice()->GetDeviceRemovedReason();
			std::cout << "Device Removed! Reason: " << std::hex << reason << std::endl;
		}
	}
	m_renderTarget->SwapBuffers();

	FlushCommandQueue();
	m_instanceCbIndex = 0;
}

void RenderEngine::BeginDrawUI()
{
	auto list = m_commandContext->GCommandList();

	list->SetPipelineState(m_pipelineStateObject->GPipelineStateUI());

	float w = m_swapChain->GViewport()->Width;
	float h = m_swapChain->GViewport()->Height;

	Matrix orthoProj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, w, h, 0.0f, 0.0f, 1.0f);

	CbPass cb;
	DirectX::XMStoreFloat4x4(&cb.gViewProj, DirectX::XMMatrixTranspose(orthoProj));

	cb.ambientLight.strength = { 1.f,1.f,1.f };

	m_passUICB->CopyData(0, cb);

	list->SetGraphicsRootConstantBufferView(1, m_passUICB->GetAddress());

	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderEngine::DrawUI(Texture* _texture, Vector2f _position, Vector2f _size, Vector4f _color, Vector4f _uvRect)
{
	auto list = m_commandContext->GCommandList();

	Matrix world = DirectX::XMMatrixScaling(_size.x, _size.y, 1.0f) * DirectX::XMMatrixTranslation(_position.x, _position.y, 0.0f);

	CbPerObject cb;
	DirectX::XMStoreFloat4x4(&cb.gWorld, DirectX::XMMatrixTranspose(world));
	cb.gUvRect = _uvRect;
	cb.gColor = _color;

	if (m_instanceCB.size() <= m_instanceCbIndex)
	{
		m_instanceCB.push_back(new ConstantBuffer<CbPerObject>());
		bool r = m_instanceCB[m_instanceCbIndex]->Init(m_renderDevice->GDevice(), m_desc->GcbvHeap(), 3 + m_instanceCbIndex);
		Log(r, "Initializing new Instance Constant Buffer");
	}

	m_instanceCB[m_instanceCbIndex]->CopyData(0, cb);

	list->SetGraphicsRootConstantBufferView(0, m_instanceCB[m_instanceCbIndex]->GetAddress());

	if (_texture && _texture->image != nullptr)
	{
		list->SetGraphicsRootDescriptorTable(2, _texture->image->gpuHandle);
	}

	if (m_quadMesh)
	{
		m_quadMesh->Bind(list);
		list->DrawIndexedInstanced((UINT)m_quadMesh->GetIndexCount(), 1, 0, 0, 0);
	}

	m_instanceCbIndex++;
}

bool RenderEngine::Resize(int _width, int _height)
{
	m_commandContext->GCommandList()->Reset(m_commandContext->GCommandAllocator(), nullptr);
	for (int i = 0; i < m_renderTarget->GCBufferCount(); i++)
	{
		if (m_renderTarget->GCurrentBackBuffer(i) == nullptr) continue;
		m_renderTarget->GCurrentBackBuffer(i)->Release();
	}
	if (m_renderTarget->GDepthStencilBuffer() != nullptr)
		m_renderTarget->GDepthStencilBuffer()->Release();

	HRESULT hr = m_swapChain->GSwapChain()->ResizeBuffers(m_renderTarget->GCBufferCount(), _width, _height, m_swapChain->GFormat(), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if(FAILED(hr))
	{
		DebugMsg("Failed to resize swapchain buffers", DebugFlag::WARNING);
		return 1;
	}

	m_renderTarget->SCurrentBackBuffer(0);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_desc->GrtvHandle());
	for (uint32 i = 0; i < m_renderTarget->GCBufferCount(); i++)
	{
		ID3D12Resource* backBuffer = nullptr;
		m_swapChain->GSwapChain()->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

		m_renderTarget->SCurrentBackBuffer(i, backBuffer);

		m_renderDevice->GDevice()->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);
		rtvHandle.Offset(1, m_desc->GCrtvSize());
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = _width;
	depthStencilDesc.Height = _height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear = {};
	optClear.Format = m_swapChain->depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ID3D12Resource* depthStencilResource = nullptr;
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	m_renderDevice->GDevice()->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(&depthStencilResource));

	// MAJ du membre de classe (comme tu l'as fait pour les backbuffers)
	m_renderTarget->SDepthStencilBuffer(depthStencilResource);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};	
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_swapChain->depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_renderDevice->GDevice()->CreateDepthStencilView(m_renderTarget->GDepthStencilBuffer(), &dsvDesc, m_desc->GdsvHandle());
	auto bar = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GDepthStencilBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandContext->GCommandList()->ResourceBarrier(1, &bar);

	m_commandContext->CloseAndExecute(m_queue);

	FlushCommandQueue();

	m_swapChain->GViewport()->TopLeftX = 0;
	m_swapChain->GViewport()->TopLeftY = 0;
	m_swapChain->GViewport()->Width = static_cast<float>(_width);
	m_swapChain->GViewport()->Height = static_cast<float>(_height);
	m_swapChain->GViewport()->MinDepth = 0.0f;
	m_swapChain->GViewport()->MaxDepth = 1.0f;

	m_swapChain->GScissorRect()->left = 0;
	m_swapChain->GScissorRect()->top = 0;
	m_swapChain->GScissorRect()->right = _width;
	m_swapChain->GScissorRect()->bottom = _height;

	return 0;
}

Vector3f RenderEngine::ScreenToWorld(const Vector2i& _position)
{

	Matrix view = BuildViewMatrix();
	Matrix proj = BuildProjMatrix();

	float width = m_swapChain->GViewport()->Width;
	float height = m_swapChain->GViewport()->Height;

	float x = (2.0f * _position.x) / width - 1.0f;
	float y = 1.0f - (2.0f * _position.y) / height; // flip Y for D3D
	float z = 1.0f; // Far plane (for direction ray)

	Vector4f clipSpacePos(x, y, z, 1.0f);
	XVector clipSpacePosX = DirectX::XMVectorSet(clipSpacePos.x, clipSpacePos.y, clipSpacePos.z, clipSpacePos.w);

	Matrix invVP = DirectX::XMMatrixInverse(nullptr, view * proj);

	XVector XworldPos4 = DirectX::XMVector4Transform(clipSpacePosX, invVP);
	Vector4f worldPos4;
	DirectX::XMStoreFloat4(&worldPos4 ,XworldPos4);
	Vector3f worldPos(worldPos4.x, worldPos4.y, worldPos4.z);
	worldPos = worldPos / worldPos4.w; // perspective divide

	Vector3f cameraPos = m_cameraTransform->position; // your camera position

	XVector worldPosX = DirectX::XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	XVector cameraPosX = DirectX::XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);

	XVector directionX = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldPosX, cameraPosX));

	return Vector3f(DirectX::XMVectorGetX(directionX), DirectX::XMVectorGetY(directionX), DirectX::XMVectorGetZ(directionX));

}

Texture* RenderEngine::CreateTextureResource(const wchar_t* path)
{
	Image* newImage = new Image();
	if (m_imageLoader->LoadFromFile(path, *newImage))
	{
		FlushCommandQueue();
		m_textureCache[path] = new Texture();
		m_textureCache[path]->image = newImage;
		return m_textureCache[path];
	}

	Log(true, "Failed to load texture");

	delete newImage;
	return nullptr;
}

void RenderEngine::CreateStructuredBuffer(uint32_t numElements, uint32_t stride, ID3D12Resource** buffer, bool isAppendConsume) {
	UINT64 bufferSize = (UINT64)numElements * stride;

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	m_renderDevice->GDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(buffer)
	);
}

void RenderEngine::CreateCounterBuffer(ID3D12Resource** counterBuffer) {
	// Un compteur fait toujours 4 octets (un uint32)
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	m_renderDevice->GDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(counterBuffer)
	);
}

void RenderEngine::CreateParticleUAVs(ParticleSystemResource* res, uint32_t maxParticles) {
	ID3D12Device* device = m_renderDevice->GDevice();

	int staticOffset = 100;
	UINT cbvSrvUavSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_desc->GcbvHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (staticOffset * cbvSrvUavSize);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_desc->GcbvHeap()->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (staticOffset * cbvSrvUavSize);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Buffer.StructureByteStride = sizeof(Particle);
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	device->CreateUnorderedAccessView(res->particleBuffer, nullptr, &uavDesc, cpuHandle);
	res->uavParticles.gpu = gpuHandle;

	cpuHandle.ptr += cbvSrvUavSize;
	gpuHandle.ptr += cbvSrvUavSize;

	D3D12_UNORDERED_ACCESS_VIEW_DESC freeListDesc = {};
	freeListDesc.Format = DXGI_FORMAT_UNKNOWN;
	freeListDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	freeListDesc.Buffer.FirstElement = 0;
	freeListDesc.Buffer.NumElements = maxParticles;
	freeListDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	freeListDesc.Buffer.CounterOffsetInBytes = 0;

	device->CreateUnorderedAccessView(res->freeListBuffer, res->counterBuffer, &freeListDesc, cpuHandle);
	res->uavFreeList.gpu = gpuHandle;
}

void RenderEngine::CreateParticleSRV(ParticleSystemResource* res, uint32_t maxParticles) {
	ID3D12Device* device = m_renderDevice->GDevice();
	UINT cbvSrvUavSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Apres les 2 UAVs (offset 100, 101), le SRV est a l'offset 102
	int srvOffset = 102;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_desc->GcbvHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (srvOffset * cbvSrvUavSize);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_desc->GcbvHeap()->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (srvOffset * cbvSrvUavSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	srvDesc.Buffer.StructureByteStride = sizeof(Particle);

	device->CreateShaderResourceView(res->particleBuffer, &srvDesc, cpuHandle);
	res->srvParticles.cpu = cpuHandle;
	res->srvParticles.gpu = gpuHandle;
}

void RenderEngine::InitializeFreeList(ParticleSystemResource* res, uint32_t maxParticles) {
	ID3D12Device* device = m_renderDevice->GDevice();
	UINT dataSize = maxParticles * sizeof(uint32_t);

	ID3D12Resource* uploadBuffer = nullptr;
	auto uploadHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

	device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	uint32_t* pData = nullptr;
	uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	for (uint32_t i = 0; i < maxParticles; ++i) {
		pData[i] = i;
	}
	uploadBuffer->Unmap(0, nullptr);

	ID3D12Resource* counterUpload = nullptr;
	auto counterBufDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t));
	device->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&counterBufDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&counterUpload));

	uint32_t* pCounter = nullptr;
	counterUpload->Map(0, nullptr, reinterpret_cast<void**>(&pCounter));
	*pCounter = maxParticles;
	counterUpload->Unmap(0, nullptr);

	auto list = m_commandContext->GCommandList();
	m_commandContext->GCommandAllocator()->Reset();
	list->Reset(m_commandContext->GCommandAllocator(), nullptr);

	auto barrierDest = CD3DX12_RESOURCE_BARRIER::Transition(res->freeListBuffer,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	list->ResourceBarrier(1, &barrierDest);

	list->CopyBufferRegion(res->freeListBuffer, 0, uploadBuffer, 0, dataSize);

	auto barrierUAV = CD3DX12_RESOURCE_BARRIER::Transition(res->freeListBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	list->ResourceBarrier(1, &barrierUAV);

	auto counterBarrierDest = CD3DX12_RESOURCE_BARRIER::Transition(res->counterBuffer,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	list->ResourceBarrier(1, &counterBarrierDest);

	list->CopyBufferRegion(res->counterBuffer, 0, counterUpload, 0, sizeof(uint32_t));

	auto counterBarrierUAV = CD3DX12_RESOURCE_BARRIER::Transition(res->counterBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	list->ResourceBarrier(1, &counterBarrierUAV);

	m_commandContext->CloseAndExecute(m_queue);
	FlushCommandQueue();

	uploadBuffer->Release();
	counterUpload->Release();
}

bool RenderEngine::FlushCommandQueue()
{
	m_fence->SFenceValue(m_fence->GCFenceValue() + 1);

	HRESULT hr = m_queue->Signal(m_fence->GFence(), m_fence->GCFenceValue());
	if (FAILED(hr))
	{
		DebugMsg("Failed to signal command queue", DebugFlag::ERROR_);
	}

	if (m_fence->GFence()->GetCompletedValue() < m_fence->GCFenceValue() )
	{
		HANDLE eventHandle = CreateEventEx(nullptr, (LPCWSTR)false, false, EVENT_ALL_ACCESS);

		m_fence->GFence()->SetEventOnCompletion(m_fence->GCFenceValue(), eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
	return 0;
}

void RenderEngine::HardInit()
{
	m_factory = new Factory();
	m_renderDevice = new RenderDevice();
	m_commandContext = new CommandContext();
	m_pipelineStateObject = new PipelineStateObject();
	m_swapChain = new SwapChain();
	m_fence = new Fence();
	m_desc = new Descriptors();
	m_renderTarget = new RenderTarget();
	m_passCB = new ConstantBuffer<CbPass>();
	m_passUICB = new ConstantBuffer<CbPass>();
	m_particleCB = new ConstantBuffer<CbParticle>();
	m_postProcessCB = new ConstantBuffer<CbPostProcess>();
	m_imageLoader = new ImageLoader();
	m_cameraTransform = new Transform();
	m_camera = new Camera();

	m_ambientLight = new Light();
	m_dirLight = new Light();
	m_spotLight = new Light();
	m_pointLight = new Light();

	Matrix Identity = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&m_world, Identity);	
}

bool RenderEngine::InitQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hr = m_renderDevice->GDevice()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue));
	if (FAILED(hr))
	{
		Log(true, "Failed to create command queue")
		return 1;
	}
	return 0;
}

Matrix RenderEngine::BuildViewMatrix() const
{
	XVector pos = DirectX::XMVectorSet(m_cameraTransform->position.x, m_cameraTransform->position.y, m_cameraTransform->position.z, 1);
	XVector foward = { m_cameraTransform->forward.x,m_cameraTransform->forward.y,m_cameraTransform->forward.z,1.0f };
	XVector target = DirectX::XMVectorAdd(pos, foward);
	XVector up = DirectX::XMVectorSet(m_cameraTransform->up.x, m_cameraTransform->up.y, m_cameraTransform->up.z, 1.0f );
	return DirectX::XMMatrixLookAtLH(pos, target, up);
}

Matrix RenderEngine::BuildProjMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovLH(
		m_camera->fov,
		m_swapChain->GViewport()->Width / m_swapChain->GViewport()->Height,
		m_camera->nearPlane,
		m_camera->farPlane);
}

void RenderEngine::UpdateParticles(IParticleSystemResource* _resource, const ParticleUpdateParams& _params)
{
	ParticleSystemResource* res = static_cast<ParticleSystemResource*>(_resource);
	auto list = m_commandContext->GCommandList();

	uint32_t actualSpawn = (_params.numToSpawn < res->maxParticles) ? _params.numToSpawn : res->maxParticles;

	CbParticle cb;
	cb.emitterPosition = _params.emitterPosition;
	cb.dt = _params.dt;
	cb.numToSpawn = _params.numToSpawn;
	cb.particleType = _params.particleType;
	cb.randomSeed = _params.randomSeed;
	cb.minVelocity = _params.minVelocity;
	cb.minLifetime = _params.minLifetime;
	cb.maxVelocity = _params.maxVelocity;
	cb.maxLifetime = _params.maxLifetime;
	cb.startColor = _params.startColor;
	cb.endColor = _params.endColor;
	cb.startSize = _params.startSize;
	cb.endSize = _params.endSize;
	cb.shapeType = _params.shapeType;
	cb.shapeRadius = _params.shapeRadius;
	cb.shapeBoxExtents = _params.shapeBoxExtents;
	cb.direction = _params.direction;
	cb.force = _params.force;

	m_particleCB->CopyData(0, cb);

	// Passer en compute pipeline
	list->SetComputeRootSignature(m_pipelineStateObject->GComputeRootSig());
	list->SetComputeRootConstantBufferView(0, m_particleCB->GetAddress());
	list->SetComputeRootDescriptorTable(1, res->uavParticles.gpu);
	list->SetComputeRootDescriptorTable(2, res->uavFreeList.gpu);

	// Phase 1 : Emission
	if (_params.numToSpawn > 0)
	{
		list->SetPipelineState(m_pipelineStateObject->GPsoParticleEmit());

		uint32_t numGroups = (_params.numToSpawn + 63) / 64;
		list->Dispatch(numGroups, 1, 1);

		D3D12_RESOURCE_BARRIER uavBarriers[2];
		uavBarriers[0] = CD3DX12_RESOURCE_BARRIER::UAV(res->particleBuffer);
		uavBarriers[1] = CD3DX12_RESOURCE_BARRIER::UAV(res->freeListBuffer);
		list->ResourceBarrier(2, uavBarriers);
	}

	// Phase 2 : Mise a jour
	list->SetPipelineState(m_pipelineStateObject->GPsoParticleUpdate());
	list->Dispatch((res->maxParticles + 63) / 64, 1, 1);

	// Transition UAV -> SRV pour le rendu
	D3D12_RESOURCE_BARRIER barriers[1];
	barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(res->particleBuffer,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	list->ResourceBarrier(1, barriers);
}

void RenderEngine::DrawParticles(IParticleSystemResource* _resource, Texture* _texture)
{
	ParticleSystemResource* res = static_cast<ParticleSystemResource*>(_resource);
	auto list = m_commandContext->GCommandList();

	list->SetGraphicsRootSignature(m_pipelineStateObject->GParticleDrawRootSig());
	list->SetPipelineState(m_pipelineStateObject->GPsoParticleDraw());
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	list->SetGraphicsRootConstantBufferView(0, m_passCB->GetAddress());
	list->SetGraphicsRootDescriptorTable(1, res->srvParticles.gpu);

	if (_texture && _texture->image != nullptr)
	{
		list->SetGraphicsRootDescriptorTable(2, _texture->image->gpuHandle);
	}

	list->DrawInstanced(4, res->maxParticles, 0, 0);
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(res->particleBuffer,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	list->ResourceBarrier(1, &barrier);
}

IPostProcessSystemRessource* RenderEngine::CreatePostProcessSystem()
{
	PostProcessResource* res = new PostProcessResource();
	ID3D12Device* device = m_renderDevice->GDevice();
	UINT cbvSrvUavSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UINT width = (UINT)m_swapChain->GViewport()->Width;
	UINT height = (UINT)m_swapChain->GViewport()->Height;

	res->desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		width, height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_NONE
	);

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);

	device->CreateCommittedResource(
		&heap, D3D12_HEAP_FLAG_NONE,
		&res->desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&res->srvResource)
	);

	auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		width, height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);
	device->CreateCommittedResource(
		&heap, D3D12_HEAP_FLAG_NONE,
		&uavDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&res->uavResource)
	);

	// --- Offset unique par post-process ---
	int srvOffset = m_nextPostProcessDescOffset++;
	int uavOffset = m_nextPostProcessDescOffset++;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_desc->GcbvHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += srvOffset * cbvSrvUavSize;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_desc->GcbvHeap()->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += srvOffset * cbvSrvUavSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvViewDesc = {};
	srvViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvViewDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(res->srvResource, &srvViewDesc, cpuHandle);
	res->srvHandle.cpu = cpuHandle;
	res->srvHandle.gpu = gpuHandle;

	cpuHandle = m_desc->GcbvHeap()->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += uavOffset * cbvSrvUavSize;
	gpuHandle = m_desc->GcbvHeap()->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += uavOffset * cbvSrvUavSize;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavViewDesc = {};
	uavViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavViewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavViewDesc.Texture2D.MipSlice = 0;
	device->CreateUnorderedAccessView(res->uavResource, nullptr, &uavViewDesc, cpuHandle);
	res->uavHandle.cpu = cpuHandle;
	res->uavHandle.gpu = gpuHandle;

	return res;
}

void RenderEngine::DrawPostProcess(IPostProcessSystemRessource* _resource, const PostProcessParams& _params, WString _shaderPath)
{
	if (m_pipelineStateObject->GPsoPostProcess(_shaderPath) == nullptr)
	{
		m_pipelineStateObject->LoadPostProcessShader(m_renderDevice->GDevice(), _shaderPath.c_str());
	}

	PostProcessResource* res = static_cast<PostProcessResource*>(_resource);
	auto list = m_commandContext->GCommandList();

	CbPostProcess cb;
	cb.resolution = Vector2f(m_swapChain->GViewport()->Width, m_swapChain->GViewport()->Height);
	cb.invResolution = Vector2f(1.0f / m_swapChain->GViewport()->Width, 1.0f / m_swapChain->GViewport()->Height);
	cb.deltaTime = _params.deltaTime;
	cb.time = _params.time;
	cb.params0 = _params.params0;
	cb.params1 = _params.params1;
	m_postProcessCB->CopyData(0, cb);

	// --- Copier backbuffer -> SRV ---
	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE); // On suppose qu'il est RENDER_TARGET en entrant
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(res->srvResource,
		res->srvState, D3D12_RESOURCE_STATE_COPY_DEST);
	list->ResourceBarrier(2, preCopyBarriers);

	list->CopyResource(res->srvResource, m_renderTarget->GCurrentBackBuffer());

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(res->srvResource,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(res->uavResource,
		res->uavState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	list->ResourceBarrier(2, postCopyBarriers);

	// --- Bind compute pipeline ---
	list->SetComputeRootSignature(m_pipelineStateObject->GPostProcessRootSig());
	list->SetPipelineState(m_pipelineStateObject->GPsoPostProcess(_shaderPath));
	list->SetComputeRootConstantBufferView(0, m_postProcessCB->GetAddress());
	list->SetComputeRootDescriptorTable(1, res->srvHandle.gpu);
	list->SetComputeRootDescriptorTable(2, res->uavHandle.gpu);

	UINT groupsX = ((UINT)m_swapChain->GViewport()->Width + 15) / 16;
	UINT groupsY = ((UINT)m_swapChain->GViewport()->Height + 15) / 16;
	list->Dispatch(groupsX, groupsY, 1);

	// --- Copier UAV -> backbuffer ---
	D3D12_RESOURCE_BARRIER preResultBarriers[2];
	preResultBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(res->uavResource,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	preResultBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	list->ResourceBarrier(2, preResultBarriers);

	list->CopyResource(m_renderTarget->GCurrentBackBuffer(), res->uavResource);

	// --- Restaurer les etats ---
	D3D12_RESOURCE_BARRIER finalBarriers[2];
	finalBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTarget->GCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET); // Le remet en RENDER_TARGET pour le prochain pass
	finalBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(res->uavResource,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
	list->ResourceBarrier(2, finalBarriers);

	// --- Memoriser les etats pour la prochaine frame ---
	res->srvState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	res->uavState = D3D12_RESOURCE_STATE_COMMON;
}