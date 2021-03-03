#include <QString>
#include <QFileDialog>
#include <QThread>
#include <QProgressBar>
#include <IPC.h>
#include "Client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	ui.scanProgressBar->hide();
	connect(this, &Client::reportOutput, ui.reportTextEdit, &QTextEdit::append);
	connect(this, &Client::setProgressBar, ui.scanProgressBar, &QProgressBar::setValue);
	connectToServer();
}

Client::~Client()
{
	ResetEvent(clientUp);
	IPC::Disconnect(hServer);
	IPC::Disconnect(hClient);
}


void Client::on_scanButton_clicked()
{
	ui.reportTextEdit->setText("Scanning in process...");
	ui.scanProgressBar->setValue(0);
	ui.scanProgressBar->show();
	QThread* scanThread = QThread::create(&Client::scanRequest, this);
	scanThread->start();
}

void Client::on_browseButton_clicked()
{
	QFileDialog* dialog = new QFileDialog(nullptr, "OpenFileDialog", "./");
	dialog->setFileMode(QFileDialog::Directory);
	dialog->show();

	if (dialog->exec())
		ui.pathLineEdit->setText(dialog->selectedFiles()[0]);
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


void Client::scanRequest()
{
	IPC::WriteUInt8(hServer, (uint8_t)CMDCODE::SCAN);
	IPC::WriteU16String(hServer, ui.pathLineEdit->text().toStdU16String());

	uint64_t fileCount = IPC::ReadUInt64(hClient);
	uint64_t scannedFilesCount = 0;

	for (uint64_t i = 0; i < fileCount; i++)
	{
		std::u16string report = IPC::ReadU16String(hClient);
		reportOutput(QString::fromUtf16(report.c_str()));
		scannedFilesCount++;
		
		int percents = (scannedFilesCount / (float)fileCount) * 100;
		setProgressBar(percents);
	}
	QString report = "Total files scanned: " + QString::number(scannedFilesCount);
	reportOutput(report);
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

