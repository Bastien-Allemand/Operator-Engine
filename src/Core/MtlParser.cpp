#include "MtlParser.h"
#include "pch.h"
#include "File.h"
#include "Utils.h"


MtlParser::MtlParser(const String& filePath)
{
	m_file = new File(filePath, File::Type::READ_ONLY);
}

MtlParser::~MtlParser()
{
	m_file->Close();
	delete(m_file);
	m_file = nullptr;
}

UnOrderedMap<String, MtlMaterial> MtlParser::Parse()
{
	UnOrderedMap<String, MtlMaterial> materials;

	MtlMaterial mat;
	String tName;
	MtlMaterial empty; // used to reset the mat
	Vector<String> data;
	while (m_file->GetCursor() < m_file->GetSize())
	{
		data = SplitString(m_file->ReadLine(), ' ');


		if (data[0] == "newmtl") // check if there is a new material
		{
			if (materialCount > 0)
			{
				materials[tName] = mat;
				mat = empty;
				tName.clear();
			}
			materialCount++;
			String name = data[1];
			for (int i = 2; i < data.size(); i++)
			{
				name += " ";
				if (data[i] == "")	continue;
				name += data[i];
			}
			tName = name;
		}
		data = EmptyRemover(data); // remove empty strings from data
		if (data.size() == 0) { continue; }

		if (data[0] == "Ns")
		{
			if (data.size() != 2)
				mat.specularExponent = 0;
			else
				mat.specularExponent = std::stof(data[1]);
		}
		if (data[0] == "Ka")
		{
			if (data.size() != 4) { mat.ambientColor = { 0,0,0 }; }
			else mat.ambientColor = {
				std::stof(data[1]),
				std::stof(data[2]),
				std::stof(data[3])
			};
		}
		if (data[0] == "Kd")
		{
			if (data.size() != 4) { mat.diffuseColor = { 0,0,0 }; }
			else mat.diffuseColor = {
				std::stof(data[1]),
				std::stof(data[2]),
				std::stof(data[3])
			};
		}
		if (data[0] == "Ks")
		{
			if (data.size() != 4) { mat.specularColor = { 0,0,0 }; }
			else mat.specularColor = {
				std::stof(data[1]),
				std::stof(data[2]),
				std::stof(data[3])
			};
		}
		if (data[0] == "Ke")
		{
			if (data.size() != 4) { mat.emissiveColor = { 0,0,0 }; }
			else mat.emissiveColor = {
				std::stof(data[1]),
				std::stof(data[2]),
				std::stof(data[3])
			};
		}
		if (data[0] == "Ni")
		{
			if (data.size() != 2) { mat.indexOfRefraction = 0; }
			else  mat.indexOfRefraction = std::stof(data[1]);
		}
		if (data[0] == "d")
		{
			if (data.size() != 2) { mat.dissolve = 0; }
			else mat.dissolve = std::stof(data[1]);
		}
		if (data[0] == "illum")
		{
			if (data.size() != 2) { mat.illuminationModel = 0; }
			else mat.illuminationModel = std::stof(data[1]);
		}
	}
	materials[tName] = mat;
	return materials;
}
