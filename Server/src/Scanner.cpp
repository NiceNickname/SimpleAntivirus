#include "Scanner.h"
#include <fstream>

Scanner::Scanner(const std::shared_ptr<Base>& base)
{
	this->base = base;

	// 1 MB buffer
	buffer.resize(bufferSize);
}

bool Scanner::Scan(const ScanObject& scanObject, std::u16string& virusName)
{
	if (scanObject.objtype == OBJTYPE::FILE)
		ScanFile(scanObject, virusName);
	else
		ScanMemory(scanObject, virusName);

	return true;
}
bool Scanner::ScanFile(const ScanObject& scanObject, std::u16string& virusName)
{
	std::ifstream fileStream((wchar_t*)scanObject.filePath.c_str(), std::ios::binary);
	memset(buffer.data(), 0, bufferSize);
	fileStream.read(buffer.data(), bufferSize);
	size_t offset = 0;
	for (size_t i = 0; i < bufferSize - minSigLength; i++)
	{
		if (i == bufferSize - maxSigLength)
		{
			updateBufferFromFile(fileStream);
			i = 0;
		}

		if (base->find((uint8_t*)buffer.data() + i, offset, scanObject.fileType, virusName))
			return true;

		offset++;
	}

	return false;
}

bool Scanner::ScanMemory(const ScanObject& scanObject, std::u16string& virusName)
{
	return false;
}

void Scanner::updateBufferFromFile(std::ifstream& file)
{
	std::copy(buffer.end() - maxSigLength, buffer.end(), buffer.begin());
	memset(buffer.data() + maxSigLength, 0, bufferSize - maxSigLength);
	file.read(buffer.data() + maxSigLength, bufferSize - maxSigLength);
}

