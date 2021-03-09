#include <thread>
#include <IPCMailslot.h>
#include <BinaryReader.h>

#include "Server.h"
#include "Scanner.h"
#include "BaseLoader.h"

Server::Server()
{
	base = std::shared_ptr<Base>(BaseLoader::load(u"Base.lsd"));
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
		serverShutDown = true;
	else if (cmdCode == (uint8_t)CMDCODE::CLIENTSHUTDOWN)
		clientShutDown = true;
	else if (cmdCode == (uint8_t)CMDCODE::SCAN)
	{
		Scanner scanner(base);
		scanner.scan(reader.readU16String(), ipc->writeHandle());
	}
}

void Server::start()
{
	std::thread ipcThread(&Server::startReading, this);
	ipcThread.join();
}
