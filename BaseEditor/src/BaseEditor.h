#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BaseEditor.h"

class BaseEditor : public QMainWindow
{
    Q_OBJECT

public:
    BaseEditor(QWidget *parent = Q_NULLPTR);
    void Read(const std::u16string& path);
    void Write(const std::u16string& path);

private slots:
    void on_pathButton_clicked();
    void on_writeButton_clicked();
    void on_readButton_clicked();
    void on_testDataButton_clicked();

private:
    Ui::BaseEditorClass ui;
};
