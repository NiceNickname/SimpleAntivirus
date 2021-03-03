#include <thread>
#include "Server.h"
#include "BaseLoader.h"
#include "Scanner.h"

Server::Server()
{
	clientUp = CreateEvent(NULL, TRUE, FALSE, TEXT("ClientUpEvent"));
	base = std::shared_ptr<Base>(BaseLoader::Load(u"Base.lsd"));
	
	
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

	if (cmdCode == (uint8_t)CMDCODE::SCAN)
	{
		Scanner scanner(base);
		scanner.Scan(IPC::ReadU16String(hServer), hClient);
	}
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

