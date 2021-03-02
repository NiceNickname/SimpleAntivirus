#pragma once

#include <stdint.h>
#include <string>

enum class OBJTYPE { FILE = 0, MEMORY};

struct ScanObject
{
	OBJTYPE objtype;

	// file attributes
	std::u16string fileType;
	std::u16string filePath;

	//memory attributes
	uint8_t* address;
	size_t size;
};