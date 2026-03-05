#pragma once

#include "../Engine/IRenderer.h"

class Mesh;
class Fence;
struct Light;
class Camera;
class Factory;
struct Texture;
class SwapChain;
class Transform;
class Descriptors;
class ImageLoader;
class RenderDevice;
class RenderTarget;
struct ID3D12Resource;
class CommandContext;
class PipelineStateObject;
struct ID3D12CommandQueue;
class ParticleSystemResource;

template<typename T>
class ConstantBuffer;

#include "Cbuffers.h"

class RenderEngine : public IRenderer
{
public:
	RenderEngine() = default;
	virtual ~RenderEngine();

	bool Init(int _width, int _height, HWND _handle) override;
	void BeginDraw() override;
	void Draw(Mesh* _mesh, Texture* _texture, Matrix _matrix) override;
	void CloseDraw() override;
	void BeginDrawUI() override;
	void DrawUI(Texture* _texture, Vector2f _position, Vector2f _size, Vector4f _color, Vector4f _uvRect = { 0.0f, 0.0f, 1.0f, 1.0f }) override;
	bool Resize(int _width, int _height) override;
	Vector3f ScreenToWorld(const Vector2i& _position) override;
	void InitMesh(Mesh& _mesh, Geometry _geo) override;
	void Update(float _dt) override;

	IParticleSystemResource* CreateParticleSystem(uint32_t _maxParticles) override;
	void UpdateParticles(IParticleSystemResource* _resource, const ParticleUpdateParams& _params) override;
	void DrawParticles(IParticleSystemResource* _resource, Texture* _texture) override;

	IPostProcessSystemRessource* CreatePostProcessSystem() override;
	void DrawPostProcess(IPostProcessSystemRessource* _resource, const PostProcessParams& _params, WString _shaderPath) override;

	float GCAspectRatio() const override;
	void SCameraTransform(Transform* _component) override { m_cameraTransform = _component; }
	Texture* LoadTexture(const wchar_t* _path) override;
	Camera* GCamera() const override { return m_camera; }
	Transform* GCameraTransform() const override { return m_cameraTransform; }
	void SCamera(Camera* _camera) { m_camera = _camera; }
	void SLight(Light* _light, int _type) override;
	bool m4xMsaaState = 0;
	uint32 m4xMsaaQuality = 0;

	Matrix BuildViewMatrix() const override;
	Matrix BuildProjMatrix() const override;

private:
	Factory* m_factory = nullptr;

	RenderDevice* m_renderDevice = nullptr;

	CommandContext* m_commandContext = nullptr;

	PipelineStateObject* m_pipelineStateObject = nullptr;

	SwapChain* m_swapChain = nullptr;

	ID3D12CommandQueue* m_queue = nullptr;

	Descriptors* m_desc = nullptr;

	RenderTarget* m_renderTarget = nullptr;

	Fence* m_fence = nullptr;

	Mesh* m_quadMesh = nullptr;

	ConstantBuffer<CbPass>* m_passCB = nullptr;
	ConstantBuffer<CbPass>* m_passUICB = nullptr;
	ConstantBuffer<CbParticle>* m_particleCB = nullptr;
	ConstantBuffer<CbPostProcess>* m_postProcessCB = nullptr;

	ImageLoader* m_imageLoader = nullptr;

	Vector<ConstantBuffer<CbPerObject>*> m_instanceCB;

	int m_instanceCbIndex = 0;

	Matrix4x4f m_world;
	Camera* m_camera = nullptr;
	Transform* m_cameraTransform = nullptr;

	Light* m_ambientLight;
	Light* m_dirLight;
	Light* m_pointLight;
	Light* m_spotLight;

	UnOrderedMap<WString, Texture*> m_textureCache;

	Texture* CreateTextureResource(const wchar_t* _path);

	void CreateStructuredBuffer(uint32_t _numElements, uint32_t _stride, ID3D12Resource** _buffer, bool _isAppendConsume);
	void CreateCounterBuffer(ID3D12Resource** _counterBuffer);
	void CreateParticleUAVs(ParticleSystemResource* _res, uint32_t _maxParticles);
	void CreateParticleSRV(ParticleSystemResource* _res, uint32_t _maxParticles);
	void InitializeFreeList(ParticleSystemResource* _res, uint32_t _maxParticles);

	int m_nextPostProcessDescOffset = 110;

	bool FlushCommandQueue();

	void HardInit();
	bool InitQueue();
};
