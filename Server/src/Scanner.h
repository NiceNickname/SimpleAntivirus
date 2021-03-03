#pragma once
#include "ScanObject.h"
#include <string>
#include "Base.h"
#include <memory>
#include <fstream>
#include "ScanEngine.h"
#include <Windows.h>

class Scanner
{
public:
	Scanner(const std::shared_ptr<Base>& base);
	~Scanner() = default;

	void Scan(const std::u16string& path, HANDLE hReportAddress);

private:
	void ScanDirectory(const std::u16string& path, HANDLE hReportAddress);

private:
	std::shared_ptr<Base> base;
	ScanEngine engine;
};