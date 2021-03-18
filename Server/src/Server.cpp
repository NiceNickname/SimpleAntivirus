#include <thread>
#include <IPCMailslot.h>
#include <BinaryReader.h>
#include <BinaryWriter.h>

#include "Server.h"
#include "Scanner.h"
#include "BaseLoader.h"
#include "Monitor.h"


HANDLE mutex;

Server::Server()
{
	mutex = CreateMutex(NULL, FALSE, L"Mutex");
	base = std::shared_ptr<Base>(BaseLoader::load(u"Base.lsd"));
	monitors.reserve(1000);
	threats = std::make_shared<ThreatList>(u"Threats.lsd");
	threats->load();

	scanner = Scanner(base, threats);
	scheduledScanner = ScheduleScanner(base, threats);
	
}


Server::~Server()
{
	CloseHandle(mutex);
}

void Server::startReading()
{
	ipc = IPC::Mailslots(u"\\\\.\\mailslot\\server", u"\\\\.\\mailslot\\client");

	bool serverShutDown = false, clientShutDown = false;

	while (!serverShutDown)
	{
		ipc->connect();

		while (!clientShutDown)
		{
			processRequest(clientShutDown, serverShutDown);

			if (serverShutDown)
				break;
		}

		clientShutDown = false;
	}
}


void Server::processRequest(bool& clientShutDown, bool& serverShutDown)
{
	BinaryReader reader(ipc);

	uint8_t cmdCode = reader.readUInt8();

	if (cmdCode == (uint8_t)CMDCODE::SERVERSHUTDOWN)
	{
		for (auto& el : monitors)
		{
			el.stop();
		}
		serverShutDown = true;
	}
	else if (cmdCode == (uint8_t)CMDCODE::CLIENTSHUTDOWN)
	{
		clientShutDown = true;
	}
	else if (cmdCode == (uint8_t)CMDCODE::SCAN) 
	{
		startScan();
	}
	else if (cmdCode == (uint8_t)CMDCODE::DELETETHREAT)
	{
		deleteRequest();
	}
	else if (cmdCode == (uint8_t)CMDCODE::QUARANTINE || cmdCode == (uint8_t)CMDCODE::UNQUARANTINE)
	{
		quarantine();
	}
	else if (cmdCode == (uint8_t)CMDCODE::MONITOR)
	{
		BinaryReader reader(ipc);
		std::u16string path = reader.readU16String();
		monitors.push_back(Monitor(path, base, threats));

		std::thread monitorThread(&Monitor::start, std::ref(monitors[monitors.size() - 1]));
		monitorThread.detach();
	}
	else if (cmdCode == (uint8_t)CMDCODE::STOPMONITOR)
	{
		BinaryReader reader(ipc);
		uint64_t index = reader.readUInt64();
		monitors[index].stop();

		monitors.erase(monitors.begin() + index);
	}
	else if (cmdCode == (uint8_t)CMDCODE::STOPSCAN)
	{
		scanner.stopScan();
		while (!scanner.scanStopped()) 
		{ 
			Sleep(1); 
		}

		BinaryWriter writer(ipc);
		bool success = true;

		writer.writeUInt8((uint8_t)success);
	}
	else if (cmdCode == (uint8_t)CMDCODE::SCHEDULESCAN)
	{
		scheduleScan();
	}
}


void Server::deleteRequest()
{
	BinaryReader reader(ipc);
	uint64_t threatIndex = reader.readUInt64();
	std::u16string threatPath = threats->get(threatIndex);

	BinaryWriter writer(ipc);
	bool success = false;

	WaitForSingleObject(mutex, INFINITE);

	if (DeleteFile((wchar_t*)threatPath.c_str()))
	{
		threats->remove(threatIndex);
		threats->save();
		success = true;
	}

	ReleaseMutex(mutex);

	writer.writeUInt8((uint8_t)success);
}

void Server::quarantine()
{
	BinaryReader reader(ipc);
	uint64_t threatIndex = reader.readUInt64();
	std::u16string threatPath = threats->get(threatIndex);

	std::fstream file((wchar_t*)threatPath.c_str());

	uint32_t header = 0;

	file.read((char*)&header, sizeof(uint32_t));
	header = ~header;
	file.seekg(0);
	file.write((char*)&header, sizeof(uint32_t));
	file.close();
}

void Server::startScan()
{
	BinaryReader reader(ipc);
	std::u16string path = reader.readU16String();
	std::u16string reportPath = reader.readU16String();

	hReportAddress = INVALID_HANDLE_VALUE;

	hReportAddress = CreateFile((LPCWSTR)reportPath.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	scanner.startScan(path, hReportAddress);
}

void Server::scheduleScan()
{
	BinaryReader reader(ipc);

	std::u16string scanPath = reader.readU16String();
	uint32_t hours = reader.readUInt32();
	uint32_t minutes = reader.readUInt32();

	scheduledScanner.setScanPath(scanPath);
	scheduledScanner.setScanTime(hours, minutes);

	scheduledScanner.start();
}

void Server::pauseMonitors(const std::u16string& path)
{
	for (auto& el : monitors)
	{
		if (path.find(el.getPath()) != std::u16string::npos)
			el.pause();
	}
}

void Server::resumeMonitors(const std::u16string& path)
{
	for (auto& el : monitors)
	{
		if (path.find(el.getPath()) != std::u16string::npos)
			el.resume();
	}
}

void Server::start()
{

	std::thread ipcThread(&Server::startReading, this);
	ipcThread.join();
}
