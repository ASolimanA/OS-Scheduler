#include <QStandardItemModel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toggle.h"
#include "processProgress.h"
#include <QTableView>
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
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    toggleSwitch = new ToggleSwitch(ui->toggle_button);
    init_process_table(ui->tableView);
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

void MainWindow::init_process_table(QTableView *tableView)
{
    // Set up the model for the table view
    QStandardItemModel *model = new QStandardItemModel(0, 3, this);
    model->setHeaderData(0, Qt::Horizontal, "Process Name");
    model->setHeaderData(1, Qt::Horizontal, "Progress");
    model->setHeaderData(2, Qt::Horizontal, "Remaining burst Time");

    // Set the model to the table view
    tableView->setModel(model);

    // Set the delegate for the progress column
    ProgressBarDelegate *delegate = new ProgressBarDelegate(this);
    tableView->setItemDelegateForColumn(1, delegate);

    // Set the column widths
    tableView->setColumnWidth(0, 200);                                            // Task column width
    tableView->setColumnWidth(1, 100);                                            // Progress column width
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Stretch the last column

    // Set the row height
    tableView->verticalHeader()->setDefaultSectionSize(30);
}

void MainWindow::connect_signals()
{
    // Connect the toggle switch signal to the slot
    connect(toggleSwitch, &ToggleSwitch::toggled, this, onToggleSwitchStateChanged);

    // Connect the add process button signal to the slot
    connect(ui->addProcessButton, &QPushButton::clicked, this, &MainWindow::on_addProcessButton_clicked);
}

void MainWindow::onToggleSwitchStateChanged(bool checked)
{
    // Handle the toggle switch state change
    if (checked)
    {
        // Change any variable or perform an action when the toggle is checked
        // TODO
    }
    else
    {
        // Change any variable or perform an action when the toggle is unchecked
        // TODO
    }
}

void MainWindow::on_addProcessButton_clicked()
{
    // Get the model from the table view
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (model)
    {
        // Add a new row to the model with sample data
        QList<QStandardItem *> items;
        // Get the process name from the line edit
        QString processName = ui->processNameLine->text();
        ui->processNameLine->clear();

        // Get the remaining burst time from the line edit
        QString remainingBurstTime = ui->timeBurstLine->text();
        ui->timeBurstLine->clear();

        // Create a new item for the process name
        items << new QStandardItem(processName) << new QStandardItem("0") << new QStandardItem(remainingBurstTime);
        model->appendRow(items);

        int burstTime = remainingBurstTime.toInt(); // Get the burst time as an integer

        // Perform any necessary integration with the new process
        // TODO: Add your logic to handle the new process here
    }
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
// {
//     lastProcess = createProcessBlock();
//     lastSize = 100;
//     QVBoxLayout* processContainer = new QVBoxLayout(lastProcess);
//     QLabel* processName = new QLabel(QString("Process %1").arg(processNumber++));
//     processName->setAlignment(Qt::AlignCenter);
//     processName->setStyleSheet("border:0; font-weight:bold;");
//     processContainer->addStretch();
//     processContainer->addWidget(processName);
//     processContainer->addStretch();
//     ganttChart->addWidget(lastProcess);
// }


// void MainWindow::on_Start_Button_clicked()
// {
//     lastSize += 100;
//     lastProcess->setFixedSize(lastSize, 50);
// }

