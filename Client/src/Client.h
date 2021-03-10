#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Client.h"
#include <Windows.h>
#include <IPC.h>
#include <ThreatList.h>

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = Q_NULLPTR);
    ~Client();

private:
    void connectToServer();
    void wakeUpServer();
    void scanRequest();
    void deleteRequest(uint64_t index);
    void quarantineRequest(uint64_t index);
    void unQuarantineRequest(uint64_t index);

signals:
    void reportOutput(const QString& value);
    void setProgressBar(int value);

private slots:
    void on_scanButton_clicked();
    void on_browseButton_clicked();
    void on_shutDownButton_clicked();
    void on_reportButton_clicked();
    void on_backButton_clicked();
    void on_deleteButton_clicked();
    void on_quarantineButton_clicked();
    void on_unQuarantineButton_clicked();

private:
    Ui::ClientClass ui;
    std::unique_ptr<ThreatList> threats;
    std::shared_ptr<IPC> ipc;
};
