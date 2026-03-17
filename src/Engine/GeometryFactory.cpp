#include "pch.h"
#include "GeometryFactory.h"
#include "Geometry.h"
#include "ObjLoader.h"

Geometry* GeometryFactory::LoadGeometry(String const path)
{
	Geometry* customGeo = nullptr;

	Vector<Vertex> vertices;
	Vector<uint32> indices;

	ObjLoader obj(path.data());
	obj.LoadObj();


	for (int i = 0; i < obj.meshs.size(); i++)
	{
		for (obj::Vertex& v : obj.vertices)
		{
			Vertex ve;
			ve.pos = { v.pos.x, v.pos.y, v.pos.z };
			ve.normal = { v.normal.x, v.normal.y, v.normal.z };
			ve.tex = { v.tex.x, v.tex.y };
			vertices.push_back(ve);
		}
		for (uint32 index : obj.meshs[i].indices)
			indices.push_back(index);
	}

	customGeo = new Geometry();
	customGeo->vertexCount = (uint32)vertices.size();
	customGeo->indexCount = (uint32)indices.size();
	customGeo->vertices = new Vertex[customGeo->vertexCount];
	customGeo->indices = new uint32[customGeo->indexCount];

	return customGeo;
}