#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Client.h"
#include <Windows.h>
#include <IPC.h>

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

signals:
    void reportOutput(const QString& value);
    void setProgressBar(int value);


private slots:
    void on_scanButton_clicked();
    void on_browseButton_clicked();
    void on_shutDownButton_clicked();

private:
    Ui::ClientClass ui;
    
    std::shared_ptr<IPC> ipc;
};
