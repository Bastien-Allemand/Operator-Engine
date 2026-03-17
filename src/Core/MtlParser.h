#pragma once

class File;

struct MtlMaterial
{
	float specularExponent = 0; // Ns
	Vector3f ambientColor = { 0,0,0 }; // Ka
	Vector3f diffuseColor = { 0,0,0 }; // Kd
	Vector3f specularColor = { 0,0,0 }; // Ks
	Vector3f emissiveColor = { 0,0,0 }; // Ke
	float indexOfRefraction = 0; // Ni
	float dissolve = 0; // d
	uint8 illuminationModel = 0; // illum
};

class MtlParser
{
public:
	MtlParser(const String& filePath);
	~MtlParser();
	uint16 materialCount = 0;
	UnOrderedMap<String, MtlMaterial> Parse();
private:
	File* m_file;
};

