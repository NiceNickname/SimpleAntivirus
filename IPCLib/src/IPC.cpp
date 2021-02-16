#include "IPC.h"
#include <stdexcept>

HANDLE IPC::CreateSlot(const std::string& name)
{
	HANDLE hSlot = CreateMailslot(name.c_str(),
		0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);

	return hSlot;
}

HANDLE IPC::ConnectToSlot(const std::string& name)
{
	HANDLE hSlot = CreateFile(name.c_str(),
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

void IPC::WriteString(HANDLE hSlot, const std::string& value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)value.c_str(), sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteChar(HANDLE hSlot, char value)
{
	DWORD bytesWritten;
	WriteFile(hSlot, (const void*)&value, sizeof(value), &bytesWritten, NULL);
}

void IPC::WriteArrayInt16(HANDLE hSlot, int16_t* values, uint32_t size)
{
	DWORD bytesWritten;

	WriteUInt32(hSlot, size);
	WriteFile(hSlot, values, sizeof(int16_t) * size, &bytesWritten, NULL);
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

std::string IPC::ReadString(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	char result[1024];
	ReadFile(hSlot, result, sizeof(result) - 1, &bytesRead, NULL);

	return std::move(std::string(result));
}

char IPC::ReadChar(HANDLE hSlot)
{
	WaitForData(hSlot);

	DWORD bytesRead;
	char result;
	ReadFile(hSlot, (void*)&result, sizeof(result), &bytesRead, NULL);

	return result;
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

