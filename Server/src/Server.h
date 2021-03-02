#pragma once
#include <IPC.h>
#include "Base.h"
#include <memory>

class Server
{
public:
	Server();
	~Server();

private:
	void startReading();
	void processRequest();
	void waitForClient();
	void processTestRequest();
	void testBase();

private:
	HANDLE hClient;
	HANDLE hServer;
	HANDLE clientUp;
	std::shared_ptr<Base> base;
};
