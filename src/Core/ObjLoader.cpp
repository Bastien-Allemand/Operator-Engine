#include "pch.h"
#include "ObjLoader.h"

#include "File.h"
#include "Utils.h"

ObjLoader::ObjLoader(String const& path) : pFile(nullptr), m_tempNormVect(), m_tempPosVect()
{
	File* file = new File(path, File::Type::READ_ONLY);
	pFile = file;
	m_path = path;
}

ObjLoader::ObjLoader(String const&& path) : pFile(nullptr), m_tempNormVect(), m_tempPosVect()
{
	File* file = new File(path, File::Type::READ_ONLY);
	pFile = file;
	m_path = path;
}

ObjLoader::~ObjLoader()
{
	delete pFile;
}

void ObjLoader::LoadObj()
{
	UnOrderedMap<String, MtlMaterial> materials;
	Vector<uint32> indices;
	uint32 last = 0;
	while (pFile->IsEnd() == false)
	{
		Vector<String> data = SplitString(pFile->ReadLine(), ' ');


		if (data[0] == "mtllib" && data.size() >= 2)
		{
			String path = data[1];
			if (data.size() > 2)
			{
				for (int i = 2; i < data.size(); i++)
				{
					path += " ";
					if (data[i] == "")
						continue;
					path += data[i];
				}
			}

			materials = LoadMtl(path);
		}

		if (data[0] == "usemtl")
		{
			if (mtlExists && data.size() >= 2)
			{
				String mtlName = data[1];
				if (data.size() > 2)
				{
					for (int i = 2; i < data.size(); i++)
					{
						mtlName += " ";
						mtlName += data[i];
					}
				}
				MTLData tmpmtldata;
				tmpmtldata.mtlId = mtlName;
				tmpmtldata.mtlIndexBegin = indices.size();
				if (mtlData.size() > 1)
				{
					mtlData[mtlData.size() - 1].mtlIndexEnd = indices.size();
				}
				mtlData.push_back(tmpmtldata);

			}
		}
		data = EmptyRemover(data);
		if (data.size() == 0) { continue; }

		if (data[0] == "v")
		{
			//ASSERT(data.size() == 4, "Invalid position" );

			float pos1 = std::stof(data[1]);
			float pos2 = std::stof(data[2]);
			float pos3 = std::stof(data[3]);
			m_tempPosVect.push_back({ pos1, pos2, pos3 });
		}
		if (data[0] == "vn")
		{
			//ASSERT(data.size() == 4, "Invalid normal" );

			float pos1 = std::stof(data[1]);
			float pos2 = std::stof(data[2]);
			float pos3 = std::stof(data[3]);
			m_tempNormVect.push_back({ pos1, pos2, pos3 });
		}
		if (data[0] == "vt")
		{
			//ASSERT(data.size() == 3, "Invalid textureCoordinate" );

			float pos1 = std::stof(data[1]);
			float pos2 = std::stof(data[2]);
			m_tempCoordVect.push_back({ pos1, pos2 });
		}
		if (data[0] == "f")
		{
			//ASSERT(data.size() >= 4, "Invalid face" );
			for (int64 i = 1; i < data.size(); i++)
			{
				Vector<String> face = SplitString((data[i]), '/');

				obj::Vertex tempVertex;

				if (face[0] != "")
					tempVertex.pos = m_tempPosVect[std::stoi(face[0]) - 1];
				if (face[1] != "")
					tempVertex.tex = m_tempCoordVect[std::stoi(face[1]) - 1];
				if (face[2] != "")
					tempVertex.normal = m_tempNormVect[std::stoi(face[2]) - 1];

				vertices.push_back(tempVertex);
			}

			if (data.size() == 4)
			{
				indices.push_back(last + 0);
				indices.push_back(last + 1);
				indices.push_back(last + 2);
				last += 3;
			}
			else if (data.size() == 5)
			{
				indices.push_back(last + 0);
				indices.push_back(last + 1);
				indices.push_back(last + 3);

				indices.push_back(last + 1);
				indices.push_back(last + 2);
				indices.push_back(last + 3);
				last += 4;
			}
			else
			{
				Vector<Vector3f> posVertices;

				for (uint8 i = 0; i < data.size() - 1; ++i)
				{
					posVertices.push_back(vertices[last + i].pos);
				}

				Vector<uint32> tempIndices = Triangulate(ProjectToPlane(posVertices));

				for (int8 i = tempIndices.size() - 1; i >= 0; --i)
				{
					indices.push_back(last + tempIndices[i]);
				}
				last += data.size() - 1;
			}
		}
	}
	meshs.push_back(obj::Mesh());
	if (mtlExists)
	{
		int count = 0;
		meshs[0].material = materials[mtlData[count].mtlId];

		for (int i = 0; i < indices.size(); i++)
		{
			if (mtlData[count].mtlIndexBegin > i)
			{
				continue;
			}
			else if (mtlData[count].mtlIndexEnd <= i)
			{
				count++;
				meshs.push_back(obj::Mesh());
				meshs[count].material = materials[mtlData[count].mtlId];
			}
			meshs[count].indices.push_back(indices[i]);
		}
	}
	else
	{
		meshs[0].material = MtlMaterial();
		meshs[0].indices = indices;
	}
}
UnOrderedMap<String, MtlMaterial> ObjLoader::LoadMtl(String& path)
{
	//load mtl if exists in the same directory as the obj
	String mtlPath = m_path.substr(0, m_path.find_last_of('/')) + "/" + path;

	File file(mtlPath, File::Type::READ_ONLY);
	if (file.IsOpened())
	{
		MtlParser mtlParser(mtlPath);
		return mtlParser.Parse();
		mtlExists = true;
	}
	return UnOrderedMap<String, MtlMaterial>();
}
void ObjLoader::FreeVectors()
{
	// Frees all vectors inside the class except m_meshVector
	m_tempIndiceVect.clear();
	m_tempNormVect.clear();
	m_tempPosVect.clear();
	m_tempCoordVect.clear();
}