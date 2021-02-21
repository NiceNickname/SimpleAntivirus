#include "IPC.h"
#include <stdexcept>

HANDLE IPC::CreateSlot(const std::u16string& name)
{
	HANDLE hSlot = CreateMailslot((LPCWSTR)name.c_str(),
		0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);

	return hSlot;
}

HANDLE IPC::ConnectToSlot(const std::u16string& name)
{
	HANDLE hSlot = CreateFile((LPCWSTR)name.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	return hSlot;
}


void IPC::Disconnect(HANDLE hSlot)
{
	CloseHandle(hSlot);
}


bool IPC::HasNewData(HANDLE hSlot)
{
	DWORD cbMessage, cMessage;
	GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbMessage, &cMessage, (LPDWORD)NULL);
	
	return cbMessage != MAILSLOT_NO_MESSAGE;
}

bool IPC::IsInvalid(HANDLE hSlot)
{
	return hSlot == INVALID_HANDLE_VALUE;
}

void IPC::WriteInt8(HANDLE hSlot, int8_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteInt16(HANDLE hSlot, int16_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteInt32(HANDLE hSlot, int32_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteInt64(HANDLE hSlot, int64_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteUInt8(HANDLE hSlot, uint8_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteUInt16(HANDLE hSlot, uint16_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteUInt32(HANDLE hSlot, uint32_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteUInt64(HANDLE hSlot, uint64_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteU16String(HANDLE hSlot, const std::u16string& value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)value.c_str(), sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteU16Char(HANDLE hSlot, char16_t value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteFloat32(HANDLE hSlot, float value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteFloat64(HANDLE hSlot, double value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteArrayInt8(HANDLE hSlot, int8_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(int8_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayInt16(HANDLE hSlot, int16_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(int16_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayInt32(HANDLE hSlot, int32_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(int32_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayInt64(HANDLE hSlot, int64_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(int64_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayUInt8(HANDLE hSlot, uint8_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(uint8_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayUInt16(HANDLE hSlot, uint16_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(uint16_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayUInt32(HANDLE hSlot, uint32_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(uint32_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayUInt64(HANDLE hSlot, uint64_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(uint64_t) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayFloat32(HANDLE hSlot, float* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(float) * size, &bytesWritten, NULL);
}

void IPC::WriteArrayFloat64(HANDLE hSlot, double* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(double) * size, &bytesWritten, NULL);
}

int8_t IPC::ReadInt8(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	int8_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

int16_t IPC::ReadInt16(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	int16_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

int32_t IPC::ReadInt32(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	int32_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

int64_t IPC::ReadInt64(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	int64_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

uint8_t IPC::ReadUInt8(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	uint8_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);
	
	return result;
}

uint16_t IPC::ReadUInt16(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	uint16_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

uint32_t IPC::ReadUInt32(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	uint32_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

uint64_t IPC::ReadUInt64(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	uint64_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

std::u16string IPC::ReadU16String(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	char16_t result[1024];
	ReadFile(hSlot, result, sizeof(result), &bytesRead, NULL);

	return std::move(result);
}

char16_t IPC::ReadU16Char(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	char16_t result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

float IPC::ReadFloat32(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	float result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

double IPC::ReadFloat64(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	double result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
}

std::vector<int8_t> IPC::ReadArrayInt8(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<int8_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(int8_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<int16_t> IPC::ReadArrayInt16(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<int16_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(int16_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<int32_t> IPC::ReadArrayInt32(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<int32_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(int32_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<int64_t> IPC::ReadArrayInt64(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<int64_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(int64_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<uint8_t> IPC::ReadArrayUInt8(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<uint8_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(uint8_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<uint16_t> IPC::ReadArrayUInt16(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<uint16_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(uint16_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<uint32_t> IPC::ReadArrayUInt32(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<uint32_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(uint32_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<uint64_t> IPC::ReadArrayUInt64(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<uint64_t> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(uint64_t) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<float> IPC::ReadArrayFloat32(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<float> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(float) * size, &bytesRead, NULL);

	return std::move(result);
}

std::vector<double> IPC::ReadArrayFloat64(HANDLE hSlot)
{
	DWORD bytesRead;
	uint32_t size = ReadUInt32(hSlot);

	std::vector<double> result;
	result.resize(size);

	WaitForData(hSlot);

	ReadFile(hSlot, result.data(), sizeof(double) * size, &bytesRead, NULL);

	return std::move(result);
}

