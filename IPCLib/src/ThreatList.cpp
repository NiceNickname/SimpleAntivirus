#include "ThreatList.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

ThreatList::ThreatList(const std::u16string& path)
{
	this->path = path;
}

void ThreatList::load()
{
	threats.resize(0);

	BinaryReader reader(path);

	if (!reader.isOpen())
		return;

	std::u16string header = reader.readU16String();

	if (header != u"Denisovich")
		return;

	uint64_t recordCount = reader.readUInt64();

	for (size_t i = 0; i < recordCount; i++)
	{
		threats.push_back(reader.readU16String());
	}


	reader.close();
}

void ThreatList::save()
{
	BinaryWriter writer(path);

	writer.writeU16String(u"Denisovich");

	writer.writeUInt64((uint64_t)threats.size());

	for (size_t i = 0; i < threats.size(); i++)
	{
		writer.writeU16String(threats[i]);
	}

	writer.close();
}

void ThreatList::add(const std::u16string& threatPath)
{
	if (std::find(threats.begin(), threats.end(), threatPath) == threats.end())
		threats.push_back(threatPath);
}

void ThreatList::remove(uint64_t index)
{
	threats.erase(threats.begin() + index);
}

