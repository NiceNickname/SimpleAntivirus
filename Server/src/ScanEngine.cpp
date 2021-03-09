#include <fstream>
#include "ScanEngine.h"


ScanEngine::ScanEngine(const std::shared_ptr<Base>& base)
{
	this->base = base;

	// 1 MB buffer
	buffer.resize(bufferSize, 0);
}

bool ScanEngine::scan(const ScanObject& scanObject, std::u16string& virusName)
{
	if (scanObject.objtype == OBJTYPE::DIRENTRY)
	{
		if (scanFile(scanObject, virusName))
			return true;
	}
	else if (scanObject.objtype == OBJTYPE::MEMORY)
	{
		if (scanMemory(scanObject, virusName))
			return true;
	}
	else if (scanObject.objtype == OBJTYPE::ZIPENTRY)
	{
		if (scanZipEntry(scanObject, virusName))
			return true;
	}

	return false;
}
bool ScanEngine::scanFile(const ScanObject& scanObject, std::u16string& virusName)
{
	std::ifstream fileStream((wchar_t*)scanObject.filePath.c_str(), std::ios::binary | std::ios::ate);

	size_t fileSize = fileStream.tellg();

	fileStream.seekg(0);

	fileStream.read(buffer.data(), bufferSize);

	size_t offset = 0;

	for (size_t i = 0; i < fileSize / bufferSize; i++)
	{
		for (size_t j = 0; j < bufferSize - maxSigLength; j++)
		{
			if (base->find((uint8_t*)buffer.data() + j, offset, scanObject.fileType, virusName))
				return true;

			offset++;
		}
		updateBufferFromFile(fileStream);
	}

	// check last chunk of data
	for (size_t i = 0; i < bufferSize - minSigLength; i++)
	{
		if (base->find((uint8_t*)buffer.data() + i, offset, scanObject.fileType, virusName))
			return true;

		offset++;
	}

	return false;
}

bool ScanEngine::scanMemory(const ScanObject& scanObject, std::u16string& virusName)
{
	virusName = u"Memory scan is not implemented yet";
	return true;
}

bool ScanEngine::scanZipEntry(const ScanObject& scanObject, std::u16string& virusName)
{
	zip_file* file = zip_fopen_index(scanObject.archive, scanObject.index, 0);

	const char* name = zip_get_name(scanObject.archive, scanObject.index, 0);

	struct zip_stat st;
	zip_stat_init(&st);
	zip_stat(scanObject.archive, name, 0, &st);


	zip_fread(file, buffer.data(), bufferSize);

	size_t offset = 0;

	for (size_t i = 0; i < st.size / bufferSize; i++)
	{
		for (size_t j = 0; j < bufferSize - maxSigLength; j++)
		{
			if (base->find((uint8_t*)buffer.data() + j, offset, scanObject.fileType, virusName))
				return true;

			offset++;
		}
		updateBufferFromZipEntry(file);
	}

	// check last chunk of data
	for (size_t i = 0; i < bufferSize - minSigLength; i++)
	{
		if (base->find((uint8_t*)buffer.data() + i, offset, scanObject.fileType, virusName))
			return true;

		offset++;
	}

	return false;
}

void ScanEngine::updateBufferFromFile(std::ifstream& file)
{
	std::copy(buffer.end() - maxSigLength, buffer.end(), buffer.begin());
	memset(buffer.data() + maxSigLength, 0, bufferSize - maxSigLength);
	file.read(buffer.data() + maxSigLength, bufferSize - maxSigLength);
}

void ScanEngine::updateBufferFromZipEntry(zip_file* file)
{
	std::copy(buffer.end() - maxSigLength, buffer.end(), buffer.begin());
	memset(buffer.data() + maxSigLength, 0, bufferSize - maxSigLength);
	zip_fread(file, buffer.data() + maxSigLength, bufferSize - maxSigLength);
}
