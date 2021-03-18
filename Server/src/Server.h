#pragma once
#include <IPC.h>
#include <memory>
#include "Base.h"
#include "ThreatList.h"
#include "Monitor.h"
#include <ctime>
#include "ScheduleScanner.h"

class Server
{
public:
	Server();
	~Server();

	void start();

private:
	void startReading();
	void processRequest(bool& clientShutDown, bool& serverShutDown);
	void deleteRequest();
	void quarantine();
	void startScan();
	void scheduleScan();

	void pauseMonitors(const std::u16string& path);
	void resumeMonitors(const std::u16string& path);
private:
	std::shared_ptr<Base> base;
	std::shared_ptr<IPC> ipc;
	std::shared_ptr<ThreatList> threats;

	Scanner scanner;
	
	std::vector<Monitor> monitors;
	HANDLE hReportAddress;

	ScheduleScanner scheduledScanner;
};
