#pragma once
#include <IPC.h>

class Server
{
public:
	Server();
	~Server();

private:
	void startReading();
	void processRequest();
	void waitForClient();

private:
	HANDLE hClient;
	HANDLE hServer;
};
