#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toggle.h"
#include "qcombobox.h"
#include "QString"

//Just Testing
QHBoxLayout* ganttChart;
QFrame* lastProcess;
int lastSize, processNumber = 1;

QFrame* createProcessBlock() {
    QFrame* process = new QFrame;
    process->setFixedSize(QSize(100, 50));
    process->setStyleSheet(QString("background-color: #3C99DC; border: 1px solid black;"));
    return process;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new ToggleSwitch(ui->toggle_button);
    QWidget* container = new QWidget;
    ganttChart = new QHBoxLayout(container);
    ganttChart->setSpacing(10);
    ganttChart->setContentsMargins(10, 5, 10, 5);
    ganttChart->setAlignment(Qt::AlignLeft);
    ui->scrollArea->setWidget(container);
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


// Implementation for Ganttchart
void MainWindow::on_Add_Button_clicked()
{
    lastProcess = createProcessBlock();
    lastSize = 100;
    QVBoxLayout* processContainer = new QVBoxLayout(lastProcess);
    QLabel* processName = new QLabel(QString("Process %1").arg(processNumber++));
    processName->setAlignment(Qt::AlignCenter);
    processName->setStyleSheet("border:0; font-weight:bold;");
    processContainer->addStretch();
    processContainer->addWidget(processName);
    processContainer->addStretch();
    ganttChart->addWidget(lastProcess);
}


void MainWindow::on_Start_Button_clicked()
{
    lastSize += 100;
    lastProcess->setFixedSize(lastSize, 50);
}

