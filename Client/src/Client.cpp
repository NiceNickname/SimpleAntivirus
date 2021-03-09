#include <QString>
#include <QFileDialog>
#include <QThread>
#include <QProgressBar>

#include <IPC.h>
#include <IPCMailslot.h>
#include <BinaryReader.h>
#include <BinaryWriter.h>
#include <TlHelp32.h>

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
	BinaryWriter reader(ipc);
	reader.writeUInt8((uint8_t)CMDCODE::CLIENTSHUTDOWN);
}

void Client::on_scanButton_clicked()
{
	ui.reportTextEdit->setText("Scanning in progress...");
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

void Client::on_shutDownButton_clicked()
{
	BinaryWriter reader(ipc);
	reader.writeUInt8((uint8_t)CMDCODE::SERVERSHUTDOWN);
	QCoreApplication::quit();
}

void Client::connectToServer()
{
	wakeUpServer();

	ipc = IPC::Mailslots(u"\\\\.\\mailslot\\client", u"\\\\.\\mailslot\\server");

	ipc->connect();
}


void Client::scanRequest()
{
	BinaryWriter writer(ipc);
	BinaryReader reader(ipc);

	writer.writeUInt8((uint8_t)CMDCODE::SCAN);
	writer.writeU16String(ui.pathLineEdit->text().toStdU16String());

	uint64_t fileCount = reader.readUInt64();
	uint64_t scannedFilesCount = 0;

	for (uint64_t i = 0; i < fileCount; i++)
	{
		std::u16string report = reader.readU16String();
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
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, L"Server.exe") == 0)
			{
				// if Server.exe is running, don't wakeup
				CloseHandle(snapshot);
				return;
			}
		}
	}

	CloseHandle(snapshot);

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

