#include <QStandardItemModel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toggle.h"
#include "processProgress.h"
#include <QTableView>
#include <QMessageBox>
#include "qcombobox.h"
#include "QString"
#include <iostream>
#include "scheduler.h"
#include "process.h"
#include "FCFS_scheduler.h"
#include "SJF_Scheduler.h"
#include "Priority_Scheduler.h"
#include "RR_SCHEDULER.h"

// Just Testing
QHBoxLayout *ganttChart;

QFrame *createProcessBlock()
{
    QFrame *process = new QFrame;
    process->setFixedSize(QSize(100, 50));
    process->setStyleSheet(QString("background-color: #3C99DC; border: 1px solid black;"));
    return process;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the window title
    setWindowTitle("OS Scheduler");

    init_gui_elements();
    // Timer
    timer = new QTimer(this);

    // Connect signals and slots
    connect_signals();
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

    // Set the selection mode and behavior
    // tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    // Set the column widths
    tableView->setColumnWidth(0, 100);                                            // Task column width
    tableView->setColumnWidth(1, 200);                                            // Progress column width
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Stretch the last column

    // Set the row height
    tableView->verticalHeader()->setDefaultSectionSize(30);
}

void MainWindow::connect_signals()
{
    // Connect the toggle switch signal to the slot
    connect(toggleSwitch, &ToggleSwitch::toggled, this, &MainWindow::onToggleSwitchStateChanged);

    // Connect the add process button signal to the slot (NOT NEEDED AS QT AUTOMATICALLY CONNECTS)
    // connect(ui->addProcessButton, &QPushButton::clicked, this, &MainWindow::on_addProcessButton_clicked);

    // Connect the start button signal to the slot (NOT NEEDED AS QT AUTOMATICALLY CONNECTS)
    // connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);

    // Connect timer to periodicFunction
    connect(timer, &QTimer::timeout, this, &MainWindow::periodicFunction);
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
        if (processName.isEmpty())
        {
            // Show a message box if the process name is empty
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid process name and other required fields.");
            return; // Return if the process name is empty
        }

        // Check if the process name already exists in the processes vector
        for (const auto &process : processes)
        {
            if (process->getName() == processName.toStdString())
            {
                // Show a message box if the process name already exists
                QMessageBox::warning(this, "Duplicate Process Name", "Process name already exists. Please choose a different name.");
                return; // Return if the process name already exists
            }
        }

        // Get the remaining burst time from the line edit
        QString remainingBurstTime = ui->timeBurstText->text();
        if (remainingBurstTime.isEmpty())
        {
            // Show a message box if the remaining burst time is empty
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid remaining burst time and other required fields.");
            return; // Return if the remaining burst time is empty
        }

        // Get Arrival time from the line edit
        QString arrivalTime = ui->arrivalText->text();
        if (arrivalTime.isEmpty())
        {
            // Show a message box if the arrival time is empty
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid arrival time and other required fields.");
            return; // Return if the arrival time is empty
        }

        // if the comboBox is not chosen as Round Robin don't get the value of lineEdit
        // QString timeQuantum;
        // int timeQuantumInt;
        // if (ui->schedulerSelect->currentText() == "Round Robin")
        // {
        //     // Get the time quantum from the line edit
        //     timeQuantum = ui->quantumText->text();
        //     timeQuantumInt = timeQuantum.toInt(); // Get the time quantum as an integer
        //     ui->quantumText->clear();
        // }

        // if the comboBox is not chosen as Round Robin or Priority don't get the value of lineEdit_3
        QString priority;
        int priorityInt;
        if (ui->schedulerSelect->currentText() == "Priority")
        {
            // Get the priority from the line edit
            priority = ui->priorityText->text();
            if (priority.isEmpty())
            {
                // Show a message box if the priority is empty
                QMessageBox::warning(this, "Invalid Input", "Please enter a valid priority.");
                return; // Return if the priority is empty
            }
            priorityInt = priority.toInt(); // Get the priority as an integer
            ui->priorityText->clear();
        }

        // Clear the line edits after getting the values
        ui->processNameLine->clear();
        ui->arrivalText->clear();
        ui->timeBurstText->clear();

        // Create a new item for the process name
        items << new QStandardItem(processName) << new QStandardItem("0") << new QStandardItem(remainingBurstTime);
        model->appendRow(items);

        int burstTime = remainingBurstTime.toInt(); // Get the burst time as an integer
        int arrivalTimeInt = arrivalTime.toInt();   // Get the arrival time as an integer

        // put the processes in the vector
        processes.push_back(std::make_shared<Process>(processName.toStdString(), arrivalTimeInt, burstTime, priorityInt));

        // close combobox
        ui->schedulerSelect->setEnabled(false);
    }
}
void MainWindow::on_schedulerSelect_currentTextChanged(const QString &arg1)
{
    if (arg1 == "FCFS" || arg1 == "Round Robin")
    {
        ui->non_preemptive->setEnabled(false);
        ui->preemptive->setEnabled(false);
        ui->non_preemptive->setChecked(true);
    }
    else
    {
        ui->non_preemptive->setEnabled(true);
        ui->preemptive->setEnabled(true);
    }
    // Enable or disable the time quantum line edit based on the selected algorithm
    if (arg1 == "Round Robin")
    {
        ui->quantumText->setEnabled(true);
    }
    else
    {
        ui->quantumText->setEnabled(false);
    }
    /// Enable or disable the priority line edit based on the selected algorithm
    if (arg1 == "Priority")
    {
        ui->priorityText->setEnabled(true);
    }
    else
    {
        ui->priorityText->setEnabled(false);
    }
}

void MainWindow::init_gui_elements()
{
    // Initialize GUI elements here
    init_process_table(ui->tableView);
    init_gantt_chart();
    init_comboBox();
    init_toggle_switch();
    init_processor_image();
}

void MainWindow::update_table_view(QTableView *tableView, const std::vector<std::shared_ptr<Process>> &processes)
{
    // Clear the existing rows in the table view
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(tableView->model());
    if (model)
    {
        model->removeRows(0, model->rowCount());
        for (const auto &process : processes)
        {
            QList<QStandardItem *> items;

            QStandardItem *nameItem = new QStandardItem(QString::fromStdString(process->getName()));

            // Progress item (non-editable)
            QStandardItem *progressItem = new QStandardItem(QString::number(process->getProgress()));
            // progressItem->setFlags(progressItem->flags() & ~Qt::ItemIsEditable); // Make this item non-editable

                        QStandardItem *remainingTimeItem = new QStandardItem(QString::number(process->getRemainingTime()));

            items << nameItem << progressItem << remainingTimeItem;
            model->appendRow(items);
        }
    }
}

void MainWindow::init_gantt_chart()
{
    // Initialize the Gantt chart layout
    QWidget *container = new QWidget;
    ganttChart = new QHBoxLayout(container);
    ganttChart->setSpacing(10);
    ganttChart->setContentsMargins(10, 5, 10, 5);
    ganttChart->setAlignment(Qt::AlignLeft);
    ui->scrollArea->setWidget(container);
}

void MainWindow::init_toggle_switch()
{
    // Initialize the toggle switch
    toggleSwitch = new ToggleSwitch(ui->toggle_button);
}

void MainWindow::init_comboBox()
{
    // comboBox
    QStringList options = {"FCFS", "SJF", "Priority", "Round Robin"};
    ui->schedulerSelect->addItems(options);
    ui->schedulerSelect->setFixedWidth(300);
    ui->schedulerSelect->setEditable(false);
}

void MainWindow::init_processor_image()
{
    // Create a graphics scene for the graphics view
    QGraphicsScene *scene = new QGraphicsScene(this);

    // Load the processor image from the resources
    QPixmap processorImage(":/assets/processor.png");

    // Check if the image was loaded successfully
    if (processorImage.isNull())
    {
        // Try alternative paths if resource path doesn't work
        processorImage.load("processor.png");

        if (processorImage.isNull())
        {
            processorImage.load("assets/processor.png");

            if (processorImage.isNull())
            {
                qDebug() << "Failed to load processor.png";
                return;
            }
        }
    }

    // Add the image to the scene
    scene->addPixmap(processorImage);

    // Set the scene to the graphics view
    ui->graphicsView->setScene(scene);

    // Scale the image to fit the view
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Disable scrollbars
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Enable antialiasing for better image quality
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

// Implementation for Ganttchart
// void MainWindow::on_Add_Button_clicked()
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

void MainWindow::updateGanttChart()
{
    static std::shared_ptr<Process> lastProcess = nullptr;
    static QFrame *lastProcessFrame = nullptr;
    static int lastSize = 0;
    std::shared_ptr<Process> currentProcess;
    if (scheduler->getCurrentProcess() != nullptr)
    {
        currentProcess = scheduler->getCurrentProcess();
    }
    else
    {
        currentProcess = nullptr;
    }
    if (currentProcess != nullptr && (lastProcess == nullptr || currentProcess != lastProcess))
    {
        lastProcess = currentProcess;
        lastProcessFrame = createProcessBlock();
        lastSize = 100;
        QVBoxLayout *processContainer = new QVBoxLayout(lastProcessFrame);
        QLabel *processName = new QLabel(QString::fromStdString(currentProcess->getName()));
        processName->setAlignment(Qt::AlignCenter);
        processName->setStyleSheet("border:0; font-weight:bold;");
        processContainer->addStretch();
        processContainer->addWidget(processName);
        processContainer->addStretch();
        ganttChart->addWidget(lastProcessFrame);
    }
    else if (lastProcessFrame != nullptr)
    {
        lastSize += 100;
        lastProcessFrame->setFixedSize(lastSize, 50);
    }
}

void MainWindow::periodicFunction()
{
    // Update Table View
    update_table_view(ui->tableView, processes);
    // Update Gantt Chart
    updateGanttChart();
    // Run the scheduler
    scheduler->runOneStep();
}

void MainWindow::finalRunUpdate()
{
    // Get average waiting time and turnaround time
    ui->avgWaitingText->setEnabled(false);
    ui->avgTurnaroundText->setEnabled(false);
    ui->avgWaitingText->setText(QString::number(scheduler->getAvgWaitingTime()));
    ui->avgTurnaroundText->setText(QString::number(scheduler->getAvgTurnaroundTime()));
}

void MainWindow::on_startButton_clicked()
{
    // Get the selected algorithm from the combo box
    QString selectedAlgorithm = ui->schedulerSelect->currentText();

    // Get the selected preemptive option
    bool isPreemptive = ui->preemptive->isChecked();

    scheduler = startScheduler(selectedAlgorithm, isPreemptive);

    // start the scheduler
    if (scheduler != nullptr)
    {
        // Check for live toggle switch
        if (toggleSwitch->isChecked())
        {
            // Start the timer with a delay for live mode
            timer->start(1000);
        }
        else
        {
            // Start the timer with no delay for non-live mode
            timer->start(10);
        }
    }
}

Scheduler *MainWindow::startScheduler(const QString &selectedAlgorithm, bool isPreemptive)
{
    Scheduler *scheduler = nullptr;
    if (selectedAlgorithm == "FCFS")
    {
        scheduler = new FCFS_Scheduler();
    }
    else if (selectedAlgorithm == "SJF")
    {
        scheduler = new SJF_Scheduler(isPreemptive);
    }
    else if (selectedAlgorithm == "Priority")
    {
        scheduler = new Priority_Scheduler(isPreemptive);
    }
    else if (selectedAlgorithm == "Round Robin")
    {
        try
        {
            int timeQuantum = ui->quantumText->text().toInt();
            scheduler = new RR_Scheduler(timeQuantum);
        }
        catch (const std::invalid_argument &e)
        {
            // Pop a message box to inform the user to enter a valid time quantum
            QMessageBox::warning(this, "Invalid Input", "Please enter a valid time quantum.");
            return nullptr; // Return null if the time quantum is invalid
        }
    }
    scheduler->addNewProcesses(processes); // Set the processes in the scheduler
    return scheduler;
}
