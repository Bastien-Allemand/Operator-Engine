#pragma once

#include "MtlParser.h"

struct MTLData
{
	String mtlId;
	uint32 mtlIndexBegin;
	uint32 mtlIndexEnd;
};
namespace obj
{
	struct Vertex
	{
		Vector3f pos;
		Vector3f normal;
		Vector2f tex;
	};

	struct Mesh
	{
		Vector <uint32> indices;
		MtlMaterial material;
	};
}
class File;

class ObjLoader
{
public:
	ObjLoader(String const& path);
	ObjLoader(String const&& path);
	~ObjLoader();

	void LoadObj();
	UnOrderedMap<String, MtlMaterial> LoadMtl(String& path);

	Vector<obj::Mesh> meshs;
	Vector<obj::Vertex> vertices;
	Vector<MTLData> mtlData;
	bool mtlExists = false;

private:
	void FreeVectors();
	File* pFile;
	String m_path;
	Vector<Vector3f> m_tempPosVect;
	Vector<Vector2f> m_tempCoordVect;
	Vector<Vector3f> m_tempNormVect;
	Vector<uint32> m_tempIndiceVect;
};

