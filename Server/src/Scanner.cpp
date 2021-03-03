#include "Scanner.h"
#include <filesystem>
#include <IPC.h>
#include <sstream>

typedef std::basic_stringstream<char16_t> u16sstream;

Scanner::Scanner(const std::shared_ptr<Base>& base)
	: engine(base)
{
	this->base = base;
}

void Scanner::Scan(const std::u16string& path, HANDLE hReportAddress)
{
	if (std::filesystem::is_directory(path))
		ScanDirectory(path, hReportAddress);	
}

void Scanner::ScanDirectory(const std::u16string& path, HANDLE hReportAddress)
{
	using std::filesystem::recursive_directory_iterator;
	using fp = bool (*)(const std::filesystem::path&);

	uint64_t numberOfFiles = std::count_if(recursive_directory_iterator(path), 
		recursive_directory_iterator{}, (fp)std::filesystem::is_regular_file);


	IPC::WriteUInt64(hReportAddress, numberOfFiles);

	for (const auto& dirEntry : recursive_directory_iterator(path))
	{
		if (std::filesystem::is_directory(dirEntry))
			continue;

		ScanObject scanObject;
		scanObject.objtype = OBJTYPE::FILE;
		scanObject.filePath = dirEntry.path().u16string();

		// delete '.' character at the start of the extension
		std::u16string fileType = dirEntry.path().extension().u16string().substr(1);

		//lowercase the extension
		std::transform(fileType.begin(), fileType.end(), fileType.begin(),
			[](char16_t c) { return std::tolower(c); });

		scanObject.fileType = fileType;

		std::u16string virusName;
		std::u16string report;
		
		u16sstream sstream;

		if (engine.Scan(scanObject, virusName))
		{
			sstream << scanObject.filePath << " found " << virusName << std::endl;
		}
		else
		{
			sstream << scanObject.filePath << " is safe " << std::endl;
		}

		IPC::WriteU16String(hReportAddress, sstream.str());
		sstream.clear();
	}
}

