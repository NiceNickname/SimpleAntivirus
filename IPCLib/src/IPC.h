#pragma once
#include <Windows.h>
#include <string>
#include <memory>


enum class TYPECODE : uint8_t
{
	UINT8 = 0, UINT16, UINT32, UINT64,
	INT8, INT16, INT32, INT64,
	FLOAT32, FLOAT64,
	U16STRING, U16CHAR,
	STRUCT,

	// arrays
	AUINT8, AUINT16, AUINT32, AUINT64,
	AINT8, AINT16, AINT32, AINT64,
	AFLOAT32, AFLOAT64
};
enum class CMDCODE : uint8_t
{
	SERVERSHUTDOWN = 0, CLIENTSHUTDOWN, SCAN
};

class IPCMailslot;

class IPC
{
public:
	static std::shared_ptr<IPCMailslot> Mailslots(const std::u16string& readPath, const std::u16string& writePath);
	virtual void connect() = 0;
	
	virtual HANDLE readHandle() = 0;
	virtual HANDLE writeHandle() = 0;

};
