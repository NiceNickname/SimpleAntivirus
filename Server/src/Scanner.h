#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <Windows.h>

#include "Base.h"
#include "ScanObject.h"
#include "ScanEngine.h"
#include "ThreatList.h"

class Scanner
{
public:
	Scanner(const std::shared_ptr<Base>& base, 
		const std::shared_ptr<ThreatList>& threats);

	~Scanner() = default;

	void scan(const std::u16string& path, HANDLE hReportAddress);

private:
	void scanDirectory(const std::u16string& path, HANDLE hReportAddress);
	void scanFile(const std::u16string& path, HANDLE hReportAddress);

	bool scanZip(const ScanObject& scanObject, std::u16string& virusName);
private:
	std::shared_ptr<Base> base;
	std::shared_ptr<ThreatList> threats;
	ScanEngine engine;
};