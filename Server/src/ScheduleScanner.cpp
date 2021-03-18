#include "ScheduleScanner.h"
#include "Windows.h"
#include <thread>



ScheduleScanner::ScheduleScanner(const std::shared_ptr<Base>& base, const std::shared_ptr<ThreatList>& threats, 
	const std::u16string path /*= u""*/, uint32_t hours /*= 0*/, uint32_t minutes /*= 0*/)
	: scanner(base, threats)
{
	scanPath = path;
	this->hours = hours;
	this->minutes = minutes;
}

ScheduleScanner& ScheduleScanner::operator=(const ScheduleScanner& other)
{
	scanner = other.scanner;
	hours = other.hours;
	minutes = other.minutes;
	scanPath = other.scanPath;

	return *this;
}

void ScheduleScanner::setScanTime(uint32_t hours, uint32_t minutes)
{
	this->hours = hours;
	this->minutes = minutes;
}

void ScheduleScanner::setScanPath(const std::u16string& path)
{
	scanPath = path;
}


void ScheduleScanner::start()
{
	std::thread scanThread(&ScheduleScanner::timeMonitoring, this);
	scanThread.detach();
}

void ScheduleScanner::timeMonitoring()
{
	SYSTEMTIME now;

	GetLocalTime(&now);

	while (now.wHour != hours || now.wMinute != minutes)
	{
		Sleep(1000);
		GetLocalTime(&now);
	}

	scanner.startScan(scanPath);
}

