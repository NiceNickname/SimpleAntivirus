#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <Windows.h>

#include "Base.h"
#include "ScanObject.h"
#include "ScanEngine.h"

class Scanner
{
public:
	Scanner(const std::shared_ptr<Base>& base);
	~Scanner() = default;

	void scan(const std::u16string& path, HANDLE hReportAddress);

private:
	void scanDirectory(const std::u16string& path, HANDLE hReportAddress);


	bool scanZip(const ScanObject& scanObject, std::u16string& virusName);
private:
	std::shared_ptr<Base> base;
	ScanEngine engine;
};