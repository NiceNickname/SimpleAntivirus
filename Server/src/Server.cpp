#include "Server.h"
#include <IPC.h>
#include <thread>
#include <iostream>
#include "BaseLoader.h"
#include <fstream>
#include "Scanner.h"

Server::Server()
{
	clientUp = CreateEvent(NULL, TRUE, FALSE, TEXT("ClientUpEvent"));
	base = std::shared_ptr<Base>(BaseLoader::Load(u"Base.lsd"));
	testBase();
	
	std::thread ipcThread(&Server::startReading, this);
	ipcThread.join();
}

Server::~Server()
{
	IPC::Disconnect(hServer);
	IPC::Disconnect(hClient);
}

void Server::startReading()
{
	hServer = IPC::CreateSlot(u"\\\\.\\mailslot\\server");

	while (true)
	{
		waitForClient();

		while (WaitForSingleObject(clientUp, 0) != WAIT_TIMEOUT)
		{
			if (!IPC::HasNewData(hServer))
				continue;

			processRequest();
		}
		
		IPC::Disconnect(hClient);
	}
}


void Server::processRequest()
{
	uint8_t cmdCode = IPC::ReadUInt8(hServer);
	if (cmdCode == (uint8_t)CMDCODE::TEST)
		processTestRequest();
}

void Server::waitForClient()
{
	hClient = IPC::ConnectToSlot(u"\\\\.\\mailslot\\client");

	while (IPC::IsInvalid(hClient))
	{
		Sleep(10);
		hClient = IPC::ConnectToSlot(u"\\\\.\\mailslot\\client");
	}

	WaitForSingleObject(clientUp, INFINITE);
}

void Server::processTestRequest()
{
	struct TestStruct
	{
		char testStringFromStruct[256];
		int16_t testNumberFromStruct;
	} testStruct;


	// getting test data from client
	uint32_t testNumber = IPC::ReadUInt32(hServer);
	std::u16string testString = IPC::ReadU16String(hServer);
	std::vector<int16_t> testVector = IPC::ReadArrayInt16(hServer);
	testStruct = IPC::ReadStruct<TestStruct>(hServer);

	// output to console
	std::cout << testNumber << std::endl;
	std::wcout << (wchar_t*)testString.c_str() << std::endl;

	for (auto el : testVector)
		std::cout << el << " ";
	std::cout << std::endl;

	std::cout << testStruct.testStringFromStruct << std::endl;
	std::cout << testStruct.testNumberFromStruct << std::endl;

	// echo to client
	IPC::WriteUInt32(hClient, testNumber);
	IPC::WriteU16String(hClient, testString);
	IPC::WriteArrayInt16(hClient, testVector.data(), testVector.size());
	IPC::WriteStruct<TestStruct>(hClient, testStruct);
}

void Server::testBase()
{
	Scanner scanner(base);

	ScanObject scanObject;
	scanObject.objtype = OBJTYPE::FILE;
	scanObject.filePath = u"exportVideo2020-12-25 16_10_46.mp4";
	scanObject.fileType = u"mp4";

	std::u16string virusName;

	if (scanner.Scan(scanObject, virusName))
		std::wcout << (wchar_t*)virusName.data() << std::endl;
	else
		std::cout << "Not a virus" << std::endl;
}

