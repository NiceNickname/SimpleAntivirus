#include "Client.h"
#include <QString>
#include <IPC.h>
#include <array>


struct TestStruct
{
	char testStringFromStruct[256];
	int16_t testNumberFromStruct;
};


Client::Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	connectToServer();
}

Client::~Client()
{
	IPC::Disconnect(hServer);
	IPC::Disconnect(hClient);
}

void Client::writeToServer()
{
	// TEST DATA
	uint32_t testNumber = 10;

	std::string testString = "TestString";

	std::array <int16_t, 5> testArray = {
		1, 2, -3, 4, 5
	};

	TestStruct testStruct = { "Hello world", 20 };
	////////////

	// send to server
	if (!IPC::IsInvalid(hServer))
	{
		IPC::WriteUInt32(hServer, testNumber);
		IPC::WriteString(hServer, testString);
		IPC::WriteArrayInt16(hServer, testArray.data(), testArray.size());
		IPC::WriteStruct<TestStruct>(hServer, testStruct);
	}
}

void Client::on_writeButton_clicked()
{
	std::thread thread(&Client::sendRequest, this);
	thread.join();
}

void Client::connectToServer()
{
	hClient = IPC::CreateSlot("\\\\.\\mailslot\\client");
	hServer = IPC::ConnectToSlot("\\\\.\\mailslot\\server");

	if (IPC::IsInvalid(hServer))
	{
		wakeUpServer();

		while (true)
		{
			hServer = IPC::ConnectToSlot("\\\\.\\mailslot\\server");

			if (!IPC::IsInvalid(hServer))
				break;

			Sleep(10);
		}
	}
}

void Client::readFromServer()
{
	if (!IPC::IsInvalid(hClient))
	{
		// read echo from server
		uint32_t testNumberResponse = IPC::ReadUInt32(hClient);
		std::string testStringResponse = IPC::ReadString(hClient);
		std::vector<int16_t> testVector = IPC::ReadArrayInt16(hClient);
		TestStruct testStructResponse = IPC::ReadStruct<TestStruct>(hClient);

		// output
		ui.uint32LineEdit->setText(QString::number(testNumberResponse));
		ui.stringLineEdit->setText(testStringResponse.c_str());
		ui.structStringLineEdit->setText(testStructResponse.testStringFromStruct);
		ui.structInt16LineEdit->setText(QString::number(testStructResponse.testNumberFromStruct));

		QString arrayResponse = "";
		for (auto el : testVector)
		{
			arrayResponse += QString::number(el) + " ";
		}

		ui.A16IntLineEdit->setText(arrayResponse);

	}
}

void Client::sendRequest()
{
	writeToServer();
	readFromServer();
}

void Client::wakeUpServer()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL,
		TEXT("Server.exe"),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
}

