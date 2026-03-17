#include "pch.h"
#include "File.h"

#include <iostream>
#include <bitset>
#include <cstdio>


File::File()
{
}
File::File(String path, Type type)
{
	Open(path, type);
}
bool File::Open(String path, Type type)
{
	m_openType = type;
	char const* mode = "   "; // ini
	switch (type)
	{
	case File::READ_ONLY:
		mode = "rb";
		break;
	case File::READ_AND_WRITE:
		mode = "rb+";
		break;
	case File::OVER_WRITE:
		mode = "wb";
		break;
	case File::READ_AND_APPEND:
		mode = "ab+";
		break;
	default:
		Log(true, "Invalid file type specified.");
	}
	if (m_pContent != nullptr)
		freopen_s(&m_pContent, path.c_str(), mode, m_pContent);
	else
		fopen_s(&m_pContent, path.c_str(), mode);
	return true;
}
//////////////////////////////////////////////////////////////////////////
/// @brief Destroys the File object and closes the file if it is open.
//////////////////////////////////////////////////////////////////////////
File::~File() 
{
	if (m_pContent != nullptr)
		Close();
}
///////////////////////////////////////////////////////////////////////////////////
/// @brief Closes the file if it is currently open.
/// @return True if the file was successfully closed, false if it was not open.
///////////////////////////////////////////////////////////////////////////////////
void File::Close()
{
	Log(m_pContent == nullptr, "File pointer is null. Cannot close file");

	fclose(m_pContent);
	m_pContent = nullptr;
}

/////////////////////////////////////////
/// @brief Gets the size of the file.
/// @return Size of the file in bytes.
/////////////////////////////////////////
uint64 File::GetSize() const
{
	Log(m_pContent == nullptr, "File pointer is null. Cannot get size.");

	long old = ftell(m_pContent);

	fseek(m_pContent, 0, SEEK_END);
	uint64 size = ftell(m_pContent);

	fseek(m_pContent, old, SEEK_SET);

	return size;
}

/////////////////////////////////////////////////////////////////////////
/// @brief Reads a line from the file.
/// @return String of the line read, including the newline character.
/////////////////////////////////////////////////////////////////////////
String File::ReadLine()
{
	Log(m_openType == Type::OVER_WRITE, "Cannot read line in OVER_WRITE mode.");
	Log(m_pContent == nullptr, "File pointer is null. Cannot read line.");

	String line;
	char ch;
	while (fread(&ch, sizeof(char), 1, m_pContent) == 1 && ch != '\n' && ch != '\0')
	{
		if (ch == '\r') continue;
		line += ch;
	}

	return line;
}

//////////////////////////////////////////
/// @brief Reads 1 byte from the file.
/// @return Byte read.
//////////////////////////////////////////
int8 File::ReadByte()
{
	Log(m_openType == Type::OVER_WRITE, "In OverWrite mode");
	Log(m_pContent == nullptr, "is File Null");
	int8 byte = 0;
	Log(fread(&byte, sizeof(int8), 1, m_pContent) != 1, "read")

	return byte;
}

//----------------------------------------------------------------------------------in Big-endian---------------------------------------------------------------//

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 16-bit integer from the file in Big-endian.
/// @return 16-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint16 File::BEReadUint16()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint16 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint16.");
	int8 bytes[2];
	Log(fread(bytes, sizeof(int8), 2, m_pContent) == 2, "Failed to read");

	return (bytes[0] << 8) | bytes[1];
}

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 32-bit integer from the file in Big-endian.
/// @return 32-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint32 File::BEReadUint32()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint32 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint32.");
	int8 bytes[4];
	Log(fread(bytes, sizeof(int8), 4, m_pContent) == 4, "Failed to read");

	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 64-bit integer from the file in Big-endian.
/// @return 64-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint64 File::BEReadUint64()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint64 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint64.");
	int8 bytes[8];
	Log(fread(bytes, sizeof(int8), 8, m_pContent) == 8, "Failed to read");
	return
		(static_cast<uint64>(bytes[0]) << 56) | (static_cast<uint64>(bytes[1]) << 48) |
		(static_cast<uint64>(bytes[2]) << 40) | (static_cast<uint64>(bytes[3]) << 32) |
		(static_cast<uint64>(bytes[4]) << 24) | (static_cast<uint64>(bytes[5]) << 16) |
		(static_cast<uint64>(bytes[6]) << 8) | static_cast<uint64>(bytes[7]);
}

//////////////////////////////////////////////////////////
/// @brief Reads a signed 8-bit integer from the file in Big-endian.
/// @return 8-bit signed integer value.
//////////////////////////////////////////////////////////
int8 File::BEReadInt8()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt8 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int8.");
	int8 byte;
	Log(fread(&byte, sizeof(int8), 1, m_pContent) == 1, "Failed to read");
	return static_cast<int8>(byte);
}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 16-bit integer from the file in Big-endian.
/// @return 16-bit signed integer value.
///////////////////////////////////////////////////////////
int16 File::BEReadInt16()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt16 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int16.");
	int8 bytes[2];
	Log(fread(bytes, sizeof(int8), 2, m_pContent) == 2, "Failed to read");
	return static_cast<int16>((bytes[0] << 8) | bytes[1]);
}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 32-bit integer from the file in Big-endian.
/// @return 32-bit signed integer value.
///////////////////////////////////////////////////////////
int32 File::BEReadInt32()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt32 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int32.");
	int8 bytes[4];
	Log(fread(bytes, sizeof(int8), 4, m_pContent) == 4, "Failed to read");
	return static_cast<int32>(bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 64-bit integer from the file in Big-endian.
/// @return 64-bit signed integer value.
///////////////////////////////////////////////////////////
int64 File::BEReadInt64()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt64 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int64.");
	int8 bytes[8];
	Log(fread(bytes, sizeof(int8), 8, m_pContent) == 8, "Failed to read");
	return
		(static_cast<int64>(bytes[0]) << 56) | (static_cast<int64>(bytes[1]) << 48) |
		(static_cast<int64>(bytes[2]) << 40) | (static_cast<int64>(bytes[3]) << 32) |
		(static_cast<int64>(bytes[4]) << 24) | (static_cast<int64>(bytes[5]) << 16) |
		(static_cast<int64>(bytes[6]) << 8) | static_cast<int64>(bytes[7]);
}

//----------------------------------------------------------------------------------in little-endian---------------------------------------------------------------//

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 16-bit integer from the file in little-endian.
/// @return 16-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint16 File::LEReadUint16()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint16 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint16.");
	int8 bytes[2];
	Log(fread(bytes, sizeof(int8), 2, m_pContent) == 2, "Failed to read");

	return static_cast<uint16>(bytes[0] | (bytes[1] << 8));
}

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 32-bit integer from the file in little-endian.
/// @return 32-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint32 File::LEReadUint32()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint32 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint32.");
	int8 bytes[4];
	Log(fread(bytes, sizeof(int8), 4, m_pContent) == 4, "Failed to read");

	return static_cast<uint32>(
		(bytes[0]) |
		(bytes[1] << 8) |
		(bytes[2] << 16) |
		(bytes[3] << 24));
}

//////////////////////////////////////////////////////////////
/// @brief Reads an unsigned 64-bit integer from the file in little-endian.
/// @return 64-bit unsigned integer value.
//////////////////////////////////////////////////////////////
uint64 File::LEReadUint64()
{
	Log(m_openType != Type::OVER_WRITE, "ReadUint64 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read uint64.");
	int8 bytes[8];
	Log(fread(bytes, sizeof(int8), 8, m_pContent) == 8, "Failed to read");
	return
		(static_cast<uint64>(bytes[0])) |
		(static_cast<uint64>(bytes[1]) << 8) |
		(static_cast<uint64>(bytes[2]) << 16) |
		(static_cast<uint64>(bytes[3]) << 24) |
		(static_cast<uint64>(bytes[4]) << 32) |
		(static_cast<uint64>(bytes[5]) << 40) |
		(static_cast<uint64>(bytes[6]) << 48) |
		(static_cast<uint64>(bytes[7]) << 56);
}

//////////////////////////////////////////////////////////
/// @brief Reads a signed 8-bit integer from the file in little-endian.
/// @return 8-bit signed integer value.
//////////////////////////////////////////////////////////
int8 File::LEReadInt8()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt8 not allowed in OVER_WRITE mode.");
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int8.");
	int8 byte;
	Log(fread(&byte, sizeof(int8), 1, m_pContent) == 1, "Failed to read");
	return static_cast<int8>(byte);
}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 16-bit integer from the file in little-endian.
/// @return 16-bit signed integer value.
///////////////////////////////////////////////////////////
int16 File::LEReadInt16()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt16 not allowed in OVER_WRITE mode." );
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int16." );
	int8 bytes[2];
	Log(fread(bytes, sizeof(int8), 2, m_pContent) == 2, "Failed to read" );
	int16 value = 0;
	value |= bytes[0];
	value |= (bytes[1] << 8);
	return static_cast<int16>(value);

}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 32-bit integer from the file in little-endian.
/// @return 32-bit signed integer value.
///////////////////////////////////////////////////////////
int32 File::LEReadInt32()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt32 not allowed in OVER_WRITE mode." );
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int32." );
	int8 bytes[4];
	Log(fread(bytes, sizeof(int8), 4, m_pContent) == 4, "Failed to read" );
	return static_cast<int32>(
		(bytes[0]) |
		(bytes[1] << 8) |
		(bytes[2] << 16) |
		(bytes[3] << 24));
}

///////////////////////////////////////////////////////////
/// @brief Reads a signed 64-bit integer from the file in little-endian.
/// @return 64-bit signed integer value.
///////////////////////////////////////////////////////////
int64 File::LEReadInt64()
{
	Log(m_openType != Type::OVER_WRITE, "ReadInt64 not allowed in OVER_WRITE mode." );
	Log(m_pContent != nullptr, "File pointer is null. Cannot read int64." );
	int8 bytes[8];
	Log(fread(bytes, sizeof(int8), 8, m_pContent) == 8, "Failed to read" );
	return
		(static_cast<int64>(bytes[0])) |
		(static_cast<int64>(bytes[1]) << 8) |
		(static_cast<int64>(bytes[2]) << 16) |
		(static_cast<int64>(bytes[3]) << 24) |
		(static_cast<int64>(bytes[4]) << 32) |
		(static_cast<int64>(bytes[5]) << 40) |
		(static_cast<int64>(bytes[6]) << 48) |
		(static_cast<int64>(bytes[7]) << 56);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Reads the entire content of the file as a string.
/// @return String containing the full content of the file, or an empty string if the file is not open.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
String File::FullRead()
{
	Log(m_openType != Type::OVER_WRITE, "FullRead not allowed in OVER_WRITE mode." );
	Log(m_pContent != nullptr, "File pointer is null. Cannot read content." );


	String content;
	char ch;
	// Read until EOF
	while (fread(&ch, sizeof(char), 1, m_pContent) == 1)
	{
		content += ch;
	}
	return content;
}

///////////////////////////////////////////
/// @brief Writes a string to the file.
/// @param insert String to write.
/// @return Number of bytes written.
///////////////////////////////////////////
size_t File::Write(String const& insert)
{
	Log(m_openType != Type::READ_ONLY, "Write(String) not allowed in READ_ONLY mode." );

	size_t written = fwrite(insert.c_str(), sizeof(char), insert.size(), m_pContent);
	return written;
}

//////////////////////////////////////////////////
/// @brief Writes a binary string to the file.
/// @param buffer Binary string to write.
//////////////////////////////////////////////////
void File::Write(void* data, size_t byteCount)
{
	Log(m_openType != Type::READ_ONLY, "Write(void*) not allowed in READ_ONLY mode." );
	Log(m_pContent != nullptr, "File pointer is null. Cannot write data." );
	size_t size = fwrite(data, 1, byteCount, m_pContent);
}

////////////////////////////////////////////////////
/// @brief Deletes a file at the specified path.
////////////////////////////////////////////////////
bool File::DeleteThisFile(char const* path)
{
	if (remove(path) != 0) return false;
	else return true;
}

////////////////////////////////////////////////////////////////////
/// @brief Rewinds the file cursor to the beginning of the file.
////////////////////////////////////////////////////////////////////
void File::RewindCursor(){	rewind(m_pContent);}

//////////////////////////////////////////////////////////////
/// @brief Sets the file cursor to the specified position.
/// @param position Cursor position.
//////////////////////////////////////////////////////////////
void File::SetCursor(uint64 position)
{
	Log(m_pContent != nullptr, "File pointer is null. Cannot set cursor position." );
	fseek(m_pContent, position, SEEK_SET);
}

///////////////////////////////////////////////////////
/// @brief Moves the file cursor by a given offset.
/// @param offset Offset to add to the cursor.
///////////////////////////////////////////////////////
void File::MoveCursor(int64 offset)
{
	Log(m_pContent != nullptr, "File pointer is null. Cannot move cursor." );
	fseek(m_pContent, offset, SEEK_CUR);
}

////////////////////////////////////////////////////////////
/// @brief Gets the current cursor position in the file.
/// @return Cursor position.
////////////////////////////////////////////////////////////
uint64 File::GetCursor() const
{
	Log(m_pContent != nullptr, "File pointer is null. Cannot get cursor position." );

	int64 pos = ftell(m_pContent);
	if (pos < 0)
	{
		//std::cerr << "Error getting cursor position\n";
		return 0;
	}
	return static_cast<uint64>(pos);
}

////////////////////////////////////////////////////////////////////////
/// @brief Indicates if the end of the file has been reached.
/// @return True if the end of the file is reached, false otherwise.
////////////////////////////////////////////////////////////////////////
bool File::IsEnd() const
{
	if (m_pContent == nullptr)
		return true;
	return feof(m_pContent) != 0;
}