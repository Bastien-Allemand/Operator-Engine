#pragma once

class Geometry;
struct Vertex;

class GeometryFactory
{

	public:
		static Geometry* CreateCubeGeo();
		static Geometry* CreateCapsuleGeo(float radius, float height, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateSphereGeo(float radius, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateHalfSphereGeo(float radius, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateIcoSphereGeo(float radius, uint32 numSubdivisions);
		static Geometry* CreateCylinderGeo(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateDonutGeo(float majorRadius, float minorRadius, uint32 majorSegments, uint32 minorSegments);
		static Geometry* CreatePlaneGeo(float width, float height);
		static Geometry* Create2DPlane();

		static Geometry* LoadGeometry(WString path);
	private:
		static Vertex MidPoint(Vertex const& v0, Vertex const& v1);
		static void Subdivide(Vector<uint32>& indices, Vector<Vertex>& vertices);
		static void BuildCylinderBody(float topRadius, float bottomRadius, float height, uint32 sliceCount, uint32 stackCount, Vector<Vertex>& vertices, Vector<uint32>& indices);
		static void BuildCylinderCap(float radius, float height, uint32 sliceCount, Vector<Vertex>& vertices, Vector<uint32>& indices, bool isTop);
		static void BuildHalfSphere(float topRadius, uint32 sliceCount, uint32 stackCount, Vector<Vertex>& vertices, Vector<uint32>& indices, bool isTop, float height = 0.0f);
	};
};

