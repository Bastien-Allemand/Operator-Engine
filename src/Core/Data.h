#pragma once

class Data
{
public:
	virtual ~Data() = default;
	virtual bool IsOpened() = 0;
	virtual void Close() = 0;
	virtual uint64 GetSize() const = 0;
	virtual String FullRead() = 0;
	virtual int8 ReadByte() = 0;
	//----------in Big-endian-------//
	virtual uint16 BEReadUint16() = 0;
	virtual uint32 BEReadUint32() = 0;
	virtual uint64 BEReadUint64() = 0;
	virtual int8 BEReadInt8() = 0;
	virtual int16 BEReadInt16() = 0;
	virtual int32 BEReadInt32() = 0;
	virtual int64 BEReadInt64() = 0;
	//---------in little-endian-----//
	virtual uint16 LEReadUint16() = 0;
	virtual uint32 LEReadUint32() = 0;
	virtual uint64 LEReadUint64() = 0;
	virtual int8 LEReadInt8() = 0;
	virtual int16 LEReadInt16() = 0;
	virtual int32 LEReadInt32() = 0;
	virtual int64 LEReadInt64() = 0;


	virtual void SetCursor(uint64 position) = 0;
	virtual void MoveCursor(int64 offset) = 0;
	virtual uint64 GetCursor() const = 0;
	virtual bool IsEnd() const = 0;
};
