#include "ScanEngine.h"
#include <fstream>

ScanEngine::ScanEngine(const std::shared_ptr<Base>& base)
{
	this->base = base;

	// 1 MB buffer
	buffer.resize(bufferSize, 0);
}

bool ScanEngine::Scan(const ScanObject& scanObject, std::u16string& virusName)
{
	if (scanObject.objtype == OBJTYPE::FILE)
	{
		if (ScanFile(scanObject, virusName))
			return true;
	}
	else
	{
		if (ScanMemory(scanObject, virusName))
			return true;
	}

	return false;
}
bool ScanEngine::ScanFile(const ScanObject& scanObject, std::u16string& virusName)
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

bool ScanEngine::ScanMemory(const ScanObject& scanObject, std::u16string& virusName)
{
	virusName = u"Memory scan is not implemented yet";
	return true;
}

void ScanEngine::updateBufferFromFile(std::ifstream& file)
{
	std::copy(buffer.end() - maxSigLength, buffer.end(), buffer.begin());
	memset(buffer.data() + maxSigLength, 0, bufferSize - maxSigLength);
	file.read(buffer.data() + maxSigLength, bufferSize - maxSigLength);
}

