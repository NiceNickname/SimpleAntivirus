#include "Client.h"
#include <QString>
#include <QThread>
#include <IPC.h>
#include <array>
#include <iostream>
#include <sstream>

struct TestStruct
{
	char testStringFromStruct[256];
	int16_t testNumberFromStruct;
};


Client::Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	connect(this, &Client::output, ui.textEdit, &QTextEdit::append);
	connectToServer();
}

Client::~Client()
{
	ResetEvent(clientUp);
	IPC::Disconnect(hServer);
	IPC::Disconnect(hClient);
}

void Client::writeToServer()
{
	
}

void Client::on_testButton_clicked()
{
	QThread* thread = QThread::create(&Client::IPCtestRequest, this);
	thread->start();
}

void Client::connectToServer()
{
	hClient = IPC::CreateSlot(u"\\\\.\\mailslot\\client");
	hServer = IPC::ConnectToSlot(u"\\\\.\\mailslot\\server");

	if (IPC::IsInvalid(hServer))
	{
		wakeUpServer();

		while (true)
		{
			hServer = IPC::ConnectToSlot(u"\\\\.\\mailslot\\server");

			if (!IPC::IsInvalid(hServer))
				break;

			Sleep(10);
		}
	}
	clientUp = OpenEvent(EVENT_ALL_ACCESS, NULL, TEXT("ClientUpEvent"));
	SetEvent(clientUp);
}

void Client::readFromServer()
{
	
}

void Client::sendRequest()
{
	writeToServer();
	readFromServer();
}

void Client::IPCtestRequest()
{
	// TEST DATA
	uint32_t testNumber = 10;

	std::u16string testString = u"TestString";

	std::array <int16_t, 5> testArray = {
		1, 2, -3, 4, 5
	};

	TestStruct testStruct = { "Hello world", 20 };
	////////////

	// send to server
	if (!IPC::IsInvalid(hServer))
	{
		QString report = "Sending to the server:\n";
		report += QString::number(testNumber) + "\n";
		report += QString::fromUtf16(testString.c_str()) + "\n";
		for (auto el : testArray)
			report += QString::number(el) + " ";
		report += "\n";
		report += testStruct.testStringFromStruct + QString("\n");
		report += QString::number(testStruct.testNumberFromStruct) + "\n";

		output(report);

		IPC::WriteUInt8(hServer, (uint8_t)CMDCODE::TEST);
		IPC::WriteUInt32(hServer, testNumber);
		IPC::WriteU16String(hServer, testString);
		IPC::WriteArrayInt16(hServer, testArray.data(), testArray.size());
		IPC::WriteStruct<TestStruct>(hServer, testStruct);
	}

	if (!IPC::IsInvalid(hClient))
	{
		// read echo from server
		uint32_t testNumberResponse = IPC::ReadUInt32(hClient);
		std::u16string testStringResponse = IPC::ReadU16String(hClient);
		std::vector<int16_t> testVector = IPC::ReadArrayInt16(hClient);
		TestStruct testStructResponse = IPC::ReadStruct<TestStruct>(hClient);
		
		// output

		QString report = "Received from the server:\n";
		report += QString::number(testNumberResponse) + "\n";
		report += QString::fromUtf16(testStringResponse.c_str()) + "\n";
		for (auto el : testVector)
			report += QString::number(el) + " ";
		report += "\n";
		report += testStructResponse.testStringFromStruct + QString("\n");
		report += QString::number(testStructResponse.testNumberFromStruct) + "\n";

		output(report);
	}
}



void Client::wakeUpServer()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	wchar_t path[256] = L"Server.exe";
	if (!CreateProcess(NULL,
		path,
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
		HRESULT error = GetLastError();
		return;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

