#include "pch.h"
#include "Utils.h"
#include <iostream>

ID3DBlob* Utils::CompileShader(const WString& _filename, const D3D_SHADER_MACRO* _defines, const String& _entryPoint, const String& _target)
{
	UINT compileFlags = 0;
	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;
	hr = D3DCompileFromFile(_filename.c_str(), _defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_entryPoint.c_str(), _target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (FAILED(hr))
	{
		Log(true, "Failed to compile shader");
		if (errors != nullptr)
		{
			std::cout << "Error: " << (char*)errors->GetBufferPointer() << std::endl;
			errors->Release();
			errors = nullptr;
		}

		return nullptr;
	}
	_Log("Shader compiled successfully");

	return byteCode;
}
