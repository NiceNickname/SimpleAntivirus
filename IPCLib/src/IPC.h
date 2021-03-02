#pragma once
#include <Windows.h>
#include <stdint.h>
#include <string>
#include <vector>

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
	TEST = 0
};

class IPC
{
public:
	// utility
	static HANDLE CreateSlot(const std::u16string& name);
	static HANDLE ConnectToSlot(const std::u16string& name);
	static void Disconnect(HANDLE hSlot);
	static bool HasNewData(HANDLE hSlot);
	static bool IsInvalid(HANDLE hSlot);
	inline static void WaitForData(HANDLE hSlot) { while (!HasNewData(hSlot)) {} }

	// WRITE FUNCTIONS

	//signed integers
	static void WriteInt8(HANDLE hSlot, int8_t value);
	static void WriteInt16(HANDLE hSlot, int16_t value);
	static void WriteInt32(HANDLE hSlot, int32_t value);
	static void WriteInt64(HANDLE hSlot, int64_t value);
	
	//unsigned integers
	static void WriteUInt8(HANDLE hSlot, uint8_t value);
	static void WriteUInt16(HANDLE hSlot, uint16_t value);
	static void WriteUInt32(HANDLE hSlot, uint32_t value);
	static void WriteUInt64(HANDLE hSlot, uint64_t value);

	// string and character
	static void WriteU16String(HANDLE hSlot, const std::u16string& value);
	static void WriteU16Char(HANDLE hSlot, char16_t value);

	static void WriteASCIIString(HANDLE hSlot, const std::string& value);
	static void WriteASCIIChar(HANDLE hSlot, char value);

	// floats
	static void WriteFloat32(HANDLE hSlot, float value);
	static void WriteFloat64(HANDLE hSlot, double value);

	// arrays
	static void WriteArrayInt8(HANDLE hSlot, int8_t* values, uint32_t size);
	static void WriteArrayInt16(HANDLE hSlot, int16_t* values, uint32_t size);
	static void WriteArrayInt32(HANDLE hSlot, int32_t* values, uint32_t size);
	static void WriteArrayInt64(HANDLE hSlot, int64_t* values, uint32_t size);

	static void WriteArrayUInt8(HANDLE hSlot, uint8_t* values, uint32_t size);
	static void WriteArrayUInt16(HANDLE hSlot, uint16_t* values, uint32_t size);
	static void WriteArrayUInt32(HANDLE hSlot, uint32_t* values, uint32_t size);
	static void WriteArrayUInt64(HANDLE hSlot, uint64_t* values, uint32_t size);

	static void WriteArrayFloat32(HANDLE hSlot, float* values, uint32_t size);
	static void WriteArrayFloat64(HANDLE hSlot, double* values, uint32_t size);


	// struct
	template<typename T>
	static void WriteStruct(HANDLE hSlot, const T& value)
	{
		DWORD bytesWritten;
		WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
	}

	// READ FUNCTIONS

	// signed integers
	static int8_t  ReadInt8(HANDLE hSlot);
	static int16_t ReadInt16(HANDLE hSlot);
	static int32_t ReadInt32(HANDLE hSlot);
	static int64_t ReadInt64(HANDLE hSlot);

	// unsigned integers
	static uint8_t ReadUInt8(HANDLE hSlot);
	static uint16_t ReadUInt16(HANDLE hSlot);
	static uint32_t ReadUInt32(HANDLE hSlot);
	static uint64_t ReadUInt64(HANDLE hSlot);
	
	// string and character
	static std::u16string ReadU16String(HANDLE hSlot);
	static char16_t ReadU16Char(HANDLE hSlot);

	static std::string ReadASCIIString(HANDLE hSlot);
	static char ReadASCIIChar(HANDLE hSlot);

	// floats
	static float ReadFloat32(HANDLE hSlot);
	static double ReadFloat64(HANDLE hSlot);

	// arrays
	static std::vector<int8_t> ReadArrayInt8(HANDLE hSlot);
	static std::vector<int16_t> ReadArrayInt16(HANDLE hSlot);
	static std::vector<int32_t> ReadArrayInt32(HANDLE hSlot);
	static std::vector<int64_t> ReadArrayInt64(HANDLE hSlot);

	static std::vector<uint8_t> ReadArrayUInt8(HANDLE hSlot);
	static std::vector<uint16_t> ReadArrayUInt16(HANDLE hSlot);
	static std::vector<uint32_t> ReadArrayUInt32(HANDLE hSlot);
	static std::vector<uint64_t> ReadArrayUInt64(HANDLE hSlot);

	static std::vector<float> ReadArrayFloat32(HANDLE hSlot);
	static std::vector<double> ReadArrayFloat64(HANDLE hSlot);
	

	// struct
	template<typename T>
	static T ReadStruct(HANDLE hSlot)
	{
		WaitForData(hSlot);

		DWORD bytesRead;
		T result;
		ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

		return result;
	}

};
