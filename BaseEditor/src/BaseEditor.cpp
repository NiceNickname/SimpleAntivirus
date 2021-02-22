#include "BaseEditor.h"
#include <IPC.h>
#include <QFileDialog>

BaseEditor::BaseEditor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void BaseEditor::Read(const std::u16string& path)
{
	HANDLE file = CreateFile((wchar_t*)path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return;

	std::u16string header = IPC::ReadU16String(file);
	if (header != std::u16string(u"Denisovich"))
		return;

	uint64_t rowCount = IPC::ReadUInt64(file);

	ui.tableWidget->setRowCount(rowCount);

	for (int i = 0; i < rowCount; i++)
	{
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(
			QString::fromUtf16(IPC::ReadU16String(file).c_str())));

		ui.tableWidget->setItem(i, 1, new QTableWidgetItem(
			QString::fromUtf16(IPC::ReadU16String(file).c_str())));

		ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(IPC::ReadUInt64(file))));
		ui.tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(IPC::ReadUInt64(file))));
		ui.tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(IPC::ReadUInt64(file))));
		ui.tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(IPC::ReadUInt64(file))));
		ui.tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(IPC::ReadUInt64(file))));
	}

	CloseHandle(file);
}




void BaseEditor::Write(const std::u16string& path)
{
	HANDLE file = CreateFile((wchar_t*)path.c_str(), GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return;

	std::u16string header(u"Denisovich");

	IPC::WriteU16String(file, header);
	IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->rowCount());

	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		std::u16string name = ui.tableWidget->item(i, 0)->data(Qt::DisplayRole).toString().toStdU16String();
		std::u16string type = ui.tableWidget->item(i, 1)->data(Qt::DisplayRole).toString().toStdU16String();
		IPC::WriteU16String(file, name);
		IPC::WriteU16String(file, type);
		IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->item(i, 2)->data(Qt::DisplayRole).toInt());
		IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->item(i, 3)->data(Qt::DisplayRole).toInt());
		IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->item(i, 4)->data(Qt::DisplayRole).toInt());
		IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->item(i, 5)->data(Qt::DisplayRole).toInt());
		IPC::WriteUInt64(file, (uint64_t)ui.tableWidget->item(i, 6)->data(Qt::DisplayRole).toInt());
	}
	CloseHandle(file);
}

void BaseEditor::on_pathButton_clicked()
{
	QFileDialog* dialog = new QFileDialog(nullptr, "OpenFileDialog", "./", "*.lsd");
	dialog->show();
	if (dialog->exec())
		ui.pathLineEdit->setText(dialog->selectedFiles()[0]);
}

void BaseEditor::on_writeButton_clicked()
{
	Write(ui.pathLineEdit->text().toStdU16String());
}

void BaseEditor::on_readButton_clicked()
{
	Read(ui.pathLineEdit->text().toStdU16String());
}

void BaseEditor::on_testDataButton_clicked()
{
	for (size_t i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem("trojan"));
		ui.tableWidget->setItem(i, 1, new QTableWidgetItem("executable"));
		ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(2020)));
		ui.tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(2323)));
		ui.tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(23)));
		ui.tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(2331)));
		ui.tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(3335)));
	}
}
