#pragma once
#include "Data.h"
#include "Define.h"

class File : public Data
{
public:

	enum Type
	{
		READ_ONLY,
		READ_AND_WRITE,
		OVER_WRITE,
		READ_AND_APPEND
	};

	File();

	File(String path, Type type = READ_ONLY);

	~File();

	bool Open(String path, Type type);

	bool IsOpened() override;

	void Close() override;


	uint64 GetSize() const override;

	String ReadLine();

	int8 ReadByte() override;

	uint16 BEReadUint16() override;
	uint32 BEReadUint32() override;
	uint64 BEReadUint64() override;
	int8 BEReadInt8() override;
	int16 BEReadInt16() override;
	int32 BEReadInt32() override;
	int64 BEReadInt64() override;

	uint16 LEReadUint16() override;
	uint32 LEReadUint32() override;
	uint64 LEReadUint64() override;
	int8 LEReadInt8() override;
	int16 LEReadInt16() override;
	int32 LEReadInt32() override;
	int64 LEReadInt64() override;

	String FullRead() override;

	template<typename T>
	uint64 Read(T* pBuffer, uint32 size);
	size_t Write(String const& insert);

	void Write(void* data, size_t byteCount);
	bool DeleteThisFile(char const* path);
	void RewindCursor();
	void SetCursor(uint64 position) override;
	void MoveCursor(int64 offset) override;
	uint64 GetCursor() const override;

	bool IsEnd() const override;

private:
	FILE* m_pContent = nullptr;
	Type m_openType;
	friend class Data;
};

template<typename T>
inline uint64 File::Read(T* pBuffer, uint32 size)
{
	if (m_pContent == nullptr)
		return 0;

	return fread(pBuffer, sizeof(T), size, m_pContent);
}

inline bool File::IsOpened()
{
	return m_pContent != nullptr;
};

