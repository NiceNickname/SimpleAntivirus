#pragma once

#include <stdint.h>
#include <string>
#include <zip.h>

enum class OBJTYPE { DIRENTRY = 0, ZIPENTRY, MEMORY};

struct ScanObject
{
	OBJTYPE objtype;

	// directory entry attributes
	std::u16string fileType;
	std::u16string filePath;

	// archive entry attributes
	zip_t* archive;
	zip_int64_t index;

	//memory attributes
	uint8_t* address;
	size_t size;

};