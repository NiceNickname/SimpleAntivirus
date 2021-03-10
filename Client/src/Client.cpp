#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QThread>
#include <QProgressBar>

#include <IPC.h>
#include <IPCMailslot.h>
#include <BinaryReader.h>
#include <BinaryWriter.h>
#include <TlHelp32.h>

#include "Client.h"

#include "FileDialog.h"


Client::Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	ui.scanProgressBar->hide();
	connect(this, &Client::reportOutput, ui.reportTextEdit, &QTextEdit::append);
	connect(this, &Client::setProgressBar, ui.scanProgressBar, &QProgressBar::setValue);
	connectToServer();
	threats = std::make_unique<ThreatList>(u"Threats.lsd");
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
	//ui.reportButton->setEnabled(false);
	QThread* scanThread = QThread::create(&Client::scanRequest, this);
	scanThread->start();
}

void Client::on_browseButton_clicked()
{
	FileDialog* dialog = new FileDialog(nullptr);
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


void Client::on_reportButton_clicked()
{
	threats->load();

	ui.threatList->clear();
	for (size_t i = 0; i < threats->size(); i++)
	{
		ui.threatList->addItem(QString::fromUtf16(threats->get(i).c_str()));
	}

	ui.stackedWidget->setCurrentIndex(1);
}

void Client::on_backButton_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void Client::on_deleteButton_clicked()
{
	uint64_t index = ui.threatList->row(ui.threatList->selectedItems()[0]);
	ui.threatList->takeItem(index);
	QThread* deleteThread = QThread::create(&Client::deleteRequest, this, index);
	deleteThread->start();
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
		std::u16string path = reader.readU16String();
		bool safe = reader.readUInt8();
		if (!safe)
		{
			std::u16string virusName = reader.readU16String();

			QString report = QString::fromUtf16(path.c_str()) + QString(" found ") + QString::fromUtf16(virusName.c_str());
			reportOutput(report);
		}
		else
		{
			QString report = QString::fromUtf16(path.c_str()) + QString(" is safe");
			reportOutput(report);
		}

		scannedFilesCount++;
		
		int percents = (scannedFilesCount / (float)fileCount) * 100;
		setProgressBar(percents);
	}
	QString report = "Total files scanned: " + QString::number(scannedFilesCount);
	reportOutput(report);
}

void Client::deleteRequest(uint64_t index)
{
	BinaryWriter writer(ipc);

	writer.writeUInt8((uint8_t)CMDCODE::DELETETHREAT);
	writer.writeUInt64(index);

	threats->remove(index);
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

