#pragma once
#include <IPC.h>
#include "Base.h"

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

private:
	HANDLE hClient;
	HANDLE hServer;
	HANDLE clientUp;
	Base base;
};
