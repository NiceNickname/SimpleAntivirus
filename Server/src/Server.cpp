#include <thread>
#include <IPCMailslot.h>
#include <BinaryReader.h>

#include "Server.h"
#include "Scanner.h"
#include "BaseLoader.h"
#include <fstream>

Server::Server()
{
	base = std::shared_ptr<Base>(BaseLoader::load(u"Base.lsd"));

	threats = std::make_shared<ThreatList>(u"Threats.lsd");
	threats->load();
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
		Scanner scanner(base, threats);
		scanner.scan(reader.readU16String(), ipc->writeHandle());
	}
	else if (cmdCode == (uint8_t)CMDCODE::DELETETHREAT)
	{
		uint64_t threatIndex = reader.readUInt64();
		std::u16string threatPath = threats->get(threatIndex);

		DeleteFile((wchar_t*)threatPath.c_str());
		
		threats->remove(threatIndex);
		threats->save();
	}
	else if (cmdCode == (uint8_t)CMDCODE::QUARANTINE || cmdCode == (uint8_t)CMDCODE::UNQUARANTINE)
	{
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
}

void Server::start()
{
	std::thread ipcThread(&Server::startReading, this);
	ipcThread.join();
}
