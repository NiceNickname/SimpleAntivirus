#include "Server.h"
#include <IPC.h>
#include <thread>
#include <iostream>

#define PIPE_SIZE 1024

Server::Server()
{
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
	hServer = IPC::CreateSlot("\\\\.\\mailslot\\server");

	while (true)
	{
		waitForClient();

		while (!IPC::IsInvalid(hServer) && !IPC::IsInvalid(hClient))
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
	struct TestStruct
	{
		char testStringFromStruct[256];
		int16_t testNumberFromStruct;
	} testStruct;

	// getting test data from client
	uint32_t testNumber = IPC::ReadUInt32(hServer);
	std::string testString = IPC::ReadString(hServer);
	std::vector<int16_t> testVector = IPC::ReadArrayInt16(hServer);
	testStruct = IPC::ReadStruct<TestStruct>(hServer);

	// output to console
	std::cout << testNumber << std::endl;
	std::cout << testString << std::endl;

	for (auto el : testVector)
		std::cout << el << " ";
	std::cout << std::endl;

	std::cout << testStruct.testStringFromStruct << std::endl;
	std::cout << testStruct.testNumberFromStruct << std::endl;

	// echo to client
	IPC::WriteUInt32(hClient, testNumber);
	IPC::WriteString(hClient, testString);
	IPC::WriteArrayInt16(hClient, testVector.data(), testVector.size());
	IPC::WriteStruct<TestStruct>(hClient, testStruct);
}

void Server::waitForClient()
{
	hClient = IPC::ConnectToSlot("\\\\.\\mailslot\\client");

	while (IPC::IsInvalid(hClient))
	{
		Sleep(10);
		hClient = IPC::ConnectToSlot("\\\\.\\mailslot\\client");
	}
}

