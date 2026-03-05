#pragma once
class Utils
{
public:
	static ID3DBlob* CompileShader(const WString& _filename, const D3D_SHADER_MACRO* _defines, const String& _entryPoint, const String& _target);
private:
	Utils() = default;
};

