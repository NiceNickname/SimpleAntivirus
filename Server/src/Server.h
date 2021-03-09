#pragma once
#include <IPC.h>
#include <memory>
#include "Base.h"

class Server
{
public:
	Server();
	~Server() = default;

	void start();

private:
	void startReading();
	void processRequest(bool& clientShutDown, bool& serverShutDown);

private:
	std::shared_ptr<Base> base;
	std::shared_ptr<IPC> ipc;
};
