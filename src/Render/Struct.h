#pragma once

struct Vertex3D
{
	Vector3f pos;
	Vector3f normal;
	Vector2f tex;
};

struct Vertex2D
{
	Vector3f pos;
	Vector2f tex;
};

struct ConstantBufferPass
{
	Matrix4x4f gWorld;
};

struct alignas(16) DirectionalLight
{
	Vector4f Ambient;  
	Vector4f Diffuse;  
	Vector4f Specular; 
	Vector3f Direction;
	float Pad;         
};

struct LightData
{
	DirectionalLight DirLight;
	Vector3f EyePosW;
	float cbPad;
	Matrix4x4f gViewProj;
};

struct CBFont
{
	Matrix4x4f gWorldViewProj;  
	Vector4f   uvOffset;        
};

struct Image
{
	ID3D12Resource* texture = nullptr;
	ID3D12Resource* uploadHeap = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
	UINT width = 0;
	UINT height = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	UINT heapIndex = 0;
	bool isLoaded = false;
};

struct DrawCommand3D
{
	class Mesh* mesh;
	Matrix4x4f  world;
};

struct DrawCommand2D
{
	Image* image;
	float x, y;           
	float width, height;  
	float uvX  = 0.0f;    
	float uvY  = 0.0f;    
	float uvW  = 1.0f;    
	float uvH  = 1.0f;    
};