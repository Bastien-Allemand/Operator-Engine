#pragma once

class Mesh;
struct Light;
class Camera;
class Geometry;
class Transform;
class IParticleSystemResource;
class IPostProcessSystemRessource;

struct Texture;

struct ParticleUpdateParams {
    Vector3f emitterPosition;
    float dt;
    uint32_t numToSpawn;
    uint32_t particleType;
	uint32_t randomSeed;
    Vector3f minVelocity;
    float minLifetime;
    Vector3f maxVelocity;
    float maxLifetime;
    Vector4f startColor;
    Vector4f endColor;
    float startSize;
    float endSize;
	int shapeType;
	float shapeRadius;
	Vector3f shapeBoxExtents;
	Vector3f direction;
	float force;
};

struct PostProcessParams {
    float time;
    float deltaTime;
    Vector4f params0;
    Vector4f params1;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool Init(int _width, int _height, HWND _handle) = 0;
    virtual void BeginDraw() = 0;
    virtual void Draw(Mesh* _mesh, Texture* _texture, const Matrix _matrix) = 0;
    virtual void CloseDraw() = 0;
	virtual void BeginDrawUI() = 0;
    virtual void DrawUI(Texture* _texture, Vector2f _position, Vector2f _size, Vector4f _color, Vector4f _uvRect = { 0.0f, 0.0f, 1.0f, 1.0f }) = 0;
    virtual bool Resize(int _width, int _height) = 0;
	virtual Vector3f ScreenToWorld(const Vector2i& _position) = 0;
	virtual float GCAspectRatio() const = 0;
    virtual void InitMesh(Mesh& _mesh, Geometry _geo) = 0;
    virtual void Update(float _dt) = 0;

    virtual IParticleSystemResource* CreateParticleSystem(uint32_t _maxParticles) = 0;
    virtual void UpdateParticles(IParticleSystemResource* _resource, const ParticleUpdateParams& _params) = 0;
    virtual void DrawParticles(IParticleSystemResource* _resource, Texture* _texture) = 0;

    virtual IPostProcessSystemRessource* CreatePostProcessSystem() = 0;
	virtual void DrawPostProcess(IPostProcessSystemRessource* _resource, const PostProcessParams& _params, WString _shaderPath) = 0;

    virtual Matrix BuildViewMatrix() const = 0;
    virtual Matrix BuildProjMatrix() const = 0;

	virtual void SLight(Light* _light,int _type) = 0;
	virtual void SCameraTransform(Transform* _component) = 0;
	virtual void SCamera(Camera* _camera) = 0;
	virtual Transform* GCameraTransform() const = 0;
	virtual Camera* GCamera() const = 0;

    virtual Texture* LoadTexture(const wchar_t* _path) = 0;
};