#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toggle.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new ToggleSwitch(ui->toggle_button);
}

MainWindow::~MainWindow()
{
    delete ui;
}
