#include <filesystem>
#include <IPC.h>
#include <sstream>
#include <zip.h>
#include <BinaryWriter.h>
#include <BinaryReader.h>

#include "Scanner.h"


#define MZHEADER 0x5a4d
#define ZIPHEADER 0x04034b50

Scanner::Scanner(const std::shared_ptr<Base>& base, 
	const std::shared_ptr<ThreatList>& threats)
	: engine(base)
{
	this->base = base;
	this->threats = threats;
}

void Scanner::scan(const std::u16string& path, HANDLE hReportAddress)
{
	threats->load();

	if (std::filesystem::is_directory(path))
		scanDirectory(path, hReportAddress);
	else
		scanFile(path, hReportAddress);

	threats->save();
}

void Scanner::scanDirectory(const std::u16string& path, HANDLE hReportAddress)
{
	using std::filesystem::recursive_directory_iterator;
	using fp = bool (*)(const std::filesystem::path&);

	uint64_t numberOfFiles = std::count_if(recursive_directory_iterator(path), 
		recursive_directory_iterator{}, (fp)std::filesystem::is_regular_file);

	BinaryWriter writer(hReportAddress);

	writer.writeUInt64(numberOfFiles);


	std::u16string virusName;


	for (const auto& dirEntry : recursive_directory_iterator(path))
	{
		bool safe = true;
		if (std::filesystem::is_directory(dirEntry))
			continue;

		std::u16string path = dirEntry.path().u16string();

		std::ifstream fileStream((wchar_t*)path.c_str(), std::ios::binary);

		uint16_t mzHeader;
		uint32_t zipHeader;

		fileStream.read((char*)&mzHeader, sizeof(uint16_t));
		fileStream.seekg(0);
		fileStream.read((char*)&zipHeader, sizeof(uint32_t));

		fileStream.close();

		if (mzHeader == MZHEADER)
		{
			ScanObject scanObject;
			scanObject.objtype = OBJTYPE::DIRENTRY;
			scanObject.filePath = path;
			scanObject.fileType = u"PE";

			if (engine.scan(scanObject, virusName))
				safe = false;
		}

		else if (zipHeader == ZIPHEADER)
		{
			ScanObject scanObject;
			scanObject.objtype = OBJTYPE::DIRENTRY;
			scanObject.filePath = path;
			scanObject.fileType = u"ZIP";


			if (scanZip(scanObject, virusName))
				safe = false;
		}

		writer.writeU16String(path);
		writer.writeUInt8((uint8_t)safe);
		if (!safe)
		{
			writer.writeU16String(virusName);
			threats->add(path);
		}
	}
}

void Scanner::scanFile(const std::u16string& path, HANDLE hReportAddress)
{
	BinaryWriter writer(hReportAddress);

	uint64_t numberOfFiles = 1;
	writer.writeUInt64(numberOfFiles);

	std::u16string virusName;

	bool safe = true;

	std::ifstream fileStream((wchar_t*)path.c_str(), std::ios::binary);

	uint16_t mzHeader;
	uint32_t zipHeader;

	fileStream.read((char*)&mzHeader, sizeof(uint16_t));
	fileStream.seekg(0);
	fileStream.read((char*)&zipHeader, sizeof(uint32_t));

	fileStream.close();

	if (mzHeader == MZHEADER)
	{
		ScanObject scanObject;
		scanObject.objtype = OBJTYPE::DIRENTRY;
		scanObject.filePath = path;
		scanObject.fileType = u"PE";

		if (engine.scan(scanObject, virusName))
			safe = false;
	}

	else if (zipHeader == ZIPHEADER)
	{
		ScanObject scanObject;
		scanObject.objtype = OBJTYPE::DIRENTRY;
		scanObject.filePath = path;
		scanObject.fileType = u"ZIP";


		if (scanZip(scanObject, virusName))
			safe = false;
	}

	writer.writeU16String(path);
	writer.writeUInt8((uint8_t)safe);
	if (!safe)
	{
		writer.writeU16String(virusName);
		threats->add(path);
	}

}

bool Scanner::scanZip(const ScanObject& scanObject, std::u16string& virusName)
{
	if (engine.scan(scanObject, virusName))
		return true;
	
	zip_source_t* src = nullptr;
	zip_t* archive = nullptr;
	zip_error_t error;

	zip_error_init(&error);

	// create archive source
	if (scanObject.objtype == OBJTYPE::DIRENTRY)
	{
		if ((src = zip_source_win32w_create((wchar_t*)scanObject.filePath.c_str(), 0, -1, &error)) == NULL) {
			fprintf(stderr, "can't create source: %s\n", zip_error_strerror(&error));
			zip_error_fini(&error);
			return NULL;
		}
	}
	else if (scanObject.objtype == OBJTYPE::ZIPENTRY)
	{
		if ((src = zip_source_zip(scanObject.archive, scanObject.archive, scanObject.index, 0, 0, 0)) == NULL) {
			fprintf(stderr, "can't create source: %s\n", zip_error_strerror(&error));
			zip_error_fini(&error);
			return NULL;
		}
	}
	else
		return false;

	/* open zip archive from source */
	if ((archive = zip_open_from_source(src, 0, &error)) == NULL) {
		fprintf(stderr, "can't open zip from source: %s\n", zip_error_strerror(&error));
		zip_source_free(src);
		zip_error_fini(&error);
		return NULL;
	}
	zip_error_fini(&error);
	

	

	zip_int64_t num_entries = zip_get_num_entries(archive, /*flags=*/0);

	for (zip_int64_t i = 0; i < num_entries; i++)
	{
		zip_file* file = zip_fopen_index(archive, i, 0);

		uint16_t mzHeader;
		uint32_t zipHeader;

		zip_fread(file, (char*)&mzHeader, sizeof(uint16_t));
		zip_fseek(file, 0, 0);

		zip_fread(file, (char*)&zipHeader, sizeof(uint32_t));
		zip_fseek(file, 0, 0);

		zip_fclose(file);

		if (mzHeader == MZHEADER)
		{
			ScanObject scanObject;
			scanObject.objtype = OBJTYPE::ZIPENTRY;
			scanObject.archive = archive;
			scanObject.index = i;
			scanObject.fileType = u"PE";

			if (engine.scan(scanObject, virusName))
				return true;
		}

		else if (zipHeader == ZIPHEADER)
		{
			ScanObject scanObject;
			scanObject.objtype = OBJTYPE::ZIPENTRY;
			scanObject.archive = archive;
			scanObject.index = i;
			scanObject.fileType = u"ZIP";

			if (scanZip(scanObject, virusName))
				return true;
		}
	}

	return false;

	
}

