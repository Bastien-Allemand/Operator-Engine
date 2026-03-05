#pragma once

struct Vertex
{
	Vector3f pos;
	Vector3f normal;
	Vector2f tex;
	Vector4f color;
};
class Geometry
{
	public:
	Geometry() = default;
	~Geometry() = default;
	Vertex* vertices;
	uint32* indices;

	uint32 vertexCount;
	uint32 indexCount;
	DirectX::BoundingSphere geoBounds;
	void BuildBox(uint32 _width = 1, uint32 _height = 1, uint32 _length = 1, Vector4f _color = { 1.0f,1.0f,1.0f,1.0f});
	void BuildSphere(uint32 _sliceCount = 2, uint32 _stackCount = 4, float _radius = 1, Vector4f _color = { 1.0f,1.0f,1.0f,1.0f });
	void BuildPlane(uint32 _width = 1,uint32 _length = 1, Vector4f _color = { 1.0f,1.0f,1.0f,1.0f });
	void BuildCylinder(uint32 _sliceCount = 2, uint32 _height = 1, Vector4f _color = { 1.0f,1.0f,1.0f,1.0f });
};

