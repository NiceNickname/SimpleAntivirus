#pragma once
#include <Windows.h>
#include <stdint.h>
#include <string>
#include <vector>

class IPC
{
public:
	// utility
	static HANDLE CreateSlot(const std::string& name);
	static HANDLE ConnectToSlot(const std::string& name);
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
	static void WriteString(HANDLE hSlot, const std::string& value);
	static void WriteChar(HANDLE hSlot, char value);

	// array
	static void WriteArrayInt16(HANDLE hSlot, int16_t* values, uint32_t size);

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
	static std::string ReadString(HANDLE hSlot);
	static char ReadChar(HANDLE hSlot);

	// arrays
	static std::vector<int16_t> ReadArrayInt16(HANDLE hSlot);

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
