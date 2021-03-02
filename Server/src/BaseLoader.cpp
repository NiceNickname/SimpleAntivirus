#include "BaseLoader.h"
#include <IPC.h>


Base* BaseLoader::Load(const std::u16string& path)
{
	wchar_t* wpath = (wchar_t*)path.c_str();
	HANDLE file = CreateFile((wchar_t*)path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return new Base();

	std::u16string header = IPC::ReadU16String(file);
	if (header != std::u16string(u"Denisovich"))
		return new Base();

	uint64_t rowCount = IPC::ReadUInt64(file);

	std::unordered_multimap<uint64_t, Record> base;
	Record record;
	for (int i = 0; i < rowCount; i++)
	{
		record.name = IPC::ReadU16String(file).c_str();
		record.type = IPC::ReadU16String(file).c_str();
		record.length = IPC::ReadUInt64(file);

		uint64_t sigStart = IPC::ReadUInt64(file);
		std::reverse((uint8_t*)&sigStart, ((uint8_t*)&sigStart) + 8);

		record.sigStart = sigStart;
		record.offsetStart = IPC::ReadUInt64(file);
		record.offsetEnd = IPC::ReadUInt64(file);
		record.sha256 = IPC::ReadASCIIString(file).c_str();

		base.insert({record.sigStart, record});
	}

	CloseHandle(file);

	return new Base(std::move(base));
}
