#pragma once

class Geometry;
struct Vertex;

class GeometryFactory
{
	public:
		static Geometry* LoadGeometry(String path);
};

