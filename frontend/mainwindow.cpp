#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toggle.h"
#include "qcombobox.h"
#include "QString"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new ToggleSwitch(ui->toggle_button);
    //comboBox
    QStringList options = {"FCFS","SJF","Priority","Round Robin"};
    ui->comboBox->addItems(options);
    ui->comboBox->setFixedWidth(300);
    ui->comboBox->setEditable(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "FCFS" || arg1 == "Round Robin") {
        ui->non_preemptive->setEnabled(false);
        ui->preemptive->setEnabled(false);
        ui->non_preemptive->setChecked(true);
    } else {
        ui->non_preemptive->setEnabled(true);
        ui->preemptive->setEnabled(true);
    }
    if (arg1 == "FCFS" || arg1 == "SJF"){
        ui->lineEdit_3->setEnabled(false);
    }
    else{
        ui->lineEdit_3->setEnabled(true);
    }
}

