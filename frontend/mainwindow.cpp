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

void MainWindow::addTimeLine() {
    QWidget *lineContainer = new QWidget;
    QVBoxLayout *lineLayout = new QVBoxLayout(lineContainer);
    lineLayout->setAlignment(Qt::AlignHCenter);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    line->setFixedHeight(40);
    line->setStyleSheet("background-color:black; margin:0;");

    countLabel = new QLabel();
    countLabel->setAlignment(Qt::AlignCenter);
    countLabel->setStyleSheet("font-weight: bold;");

    lineLayout->addWidget(line);
    lineLayout->addWidget(countLabel);

    ganttChart->addWidget(lineContainer);
}


QFrame *createProcessBlock()
{
    QFrame *process = new QFrame;
    process->setFixedSize(QSize(100, 50));
    process->setStyleSheet(QString("background-color: #3C99DC; border: 1px solid black;"));
    return process;
}

QFrame *createIdealBlock()
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
    QStandardItemModel *model = new QStandardItemModel(0, 4, this);
    model->setHeaderData(0, Qt::Horizontal, "Process Name");
    model->setHeaderData(1, Qt::Horizontal, "Progress");
    model->setHeaderData(2, Qt::Horizontal, "Remaining burst Time");
    model->setHeaderData(3, Qt::Horizontal, "Arrival Time");

    // Set the model to the table view
    tableView->setModel(model);

    // Set the delegate for the progress column
    ProgressBarDelegate *delegate = new ProgressBarDelegate(this);
    tableView->setItemDelegateForColumn(1, delegate);

    // Set the column widths
    tableView->setColumnWidth(0, 90);  // Task column width
    tableView->setColumnWidth(2, 135); // Burst time width
    tableView->setColumnWidth(3, 90);  // Arrival time width

    // Alternative approach: Set all columns to Fixed first, then override for the Progress column
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

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

    // Connect the table view model's dataChanged signal to the slot
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (model)
    {
        connect(model, &QStandardItemModel::dataChanged, this, &MainWindow::onDataChanged);
    }
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
        items << new QStandardItem(processName)
              << new QStandardItem("0")
              << new QStandardItem(remainingBurstTime)
              << new QStandardItem(arrivalTime);
        model->appendRow(items);

        int burstTime = remainingBurstTime.toInt(); // Get the burst time as an integer
        int arrivalTimeInt = arrivalTime.toInt();   // Get the arrival time as an integer

        // put the processes in the vector
        processes.push_back(std::make_shared<Process>(processName.toStdString(), arrivalTimeInt, burstTime, priorityInt));
        if (running_flag)
        {
            scheduler->addProcess(std::make_shared<Process>(processName.toStdString(), arrivalTimeInt, burstTime, priorityInt));
        }

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

            // Progress item
            QStandardItem *progressItem = new QStandardItem(QString::number(process->getProgress()));

            QStandardItem *remainingTimeItem = new QStandardItem(QString::number(process->getRemainingTime()));

            // Add arrival time item
            QStandardItem *arrivalTimeItem = new QStandardItem(QString::number(process->getArrivalTime()));

            items << nameItem << progressItem << remainingTimeItem << arrivalTimeItem;
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
                // qDebug() << "Failed to load processor.png";
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

void MainWindow::updateGanttChart()
{
    std::shared_ptr<Process> currentProcess;
    if (scheduler->getCurrentProcess() != nullptr)
    {
        currentProcess = scheduler->getCurrentProcess();
    }
    else
    {
        currentProcess = nullptr;
        GanttLastProcessFrame = nullptr;
    }
    if (currentProcess != nullptr && (GanttLastProcess == nullptr || currentProcess != GanttLastProcess))
    {
        GanttLastProcess = currentProcess;
        GanttLastProcessFrame = createProcessBlock();
        GanttLastSize = 100;
        QVBoxLayout *processContainer = new QVBoxLayout(GanttLastProcessFrame);
        QLabel *processName = new QLabel(QString::fromStdString(currentProcess->getName()));
        processName->setAlignment(Qt::AlignCenter);
        processName->setStyleSheet("border:0; font-weight:bold;");
        processContainer->addStretch();
        processContainer->addWidget(processName);
        processContainer->addStretch();
        ganttChart->addWidget(GanttLastProcessFrame);

        addTimeLine();
    }
    else if (GanttLastProcessFrame != nullptr)
    {
        GanttLastSize += 100;
        GanttLastProcessFrame->setFixedSize(GanttLastSize, 50);
    }
    else if (currentProcess == nullptr && GanttLastProcessFrame == nullptr && timeCounter != 0)
    {
        // CPU is idle — create an idle block
        GanttLastProcess = nullptr; // mark it as idle time
        GanttLastProcessFrame = createIdealBlock();
        GanttLastSize = 100;

        QVBoxLayout *idealContainer = new QVBoxLayout(GanttLastProcessFrame);
        QLabel *idealName = new QLabel("Idle");
        idealName->setAlignment(Qt::AlignCenter);
        idealName->setStyleSheet("border:0; font-weight:bold;");
        idealContainer->addStretch();
        idealContainer->addWidget(idealName);
        idealContainer->addStretch();

        ganttChart->addWidget(GanttLastProcessFrame);
        addTimeLine(); // helper to add line + time label
    }

    if (countLabel != nullptr)
    {
        countLabel->setText(QString::number(timeCounter));
    }
    timeCounter++;
}

void MainWindow::periodicFunction()
{
    if (scheduler->isSimulationComplete())
    {
        timer->stop();
        finalRunUpdate();
    }
    // Update Table View
    update_table_view(ui->tableView, processes);
    // Update Gantt Chart
    updateGanttChart();
    // Run the scheduler
    scheduler->runOneStep();
}

void MainWindow::finalRunUpdate()
{
    finished = true;
    running_flag = false;
    // Get average waiting time and turnaround time
    ui->avgWaitingText->setText(QString::number(scheduler->getAvgWaitingTime()));
    ui->avgTurnaroundText->setText(QString::number(scheduler->getAvgTurnaroundTime()));
}

void MainWindow::on_startButton_clicked()
{
    if (processes.empty())
    {
        QMessageBox::warning(this, "Invalid Operation", "No Processes to be scheduled.");
        return;
    }
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
            live = true;
        }
        else
        {
            // Start the timer with no delay for non-live mode
            timer->start(10);
            live = false;
        }
    }
    finished = false;
    running_flag = true;
    // Disable the Delete button
    ui->deleteButton->setEnabled(false);
    // Disable startButton
    ui->startButton->setEnabled(false);
    // Disable RaidoButtons ( Preemptive and non-Preemptive )
    ui->preemptive->setEnabled(false);
    ui->non_preemptive->setEnabled(false);
    // Disable editing
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

void MainWindow::on_deleteButton_clicked()
{
    if (!processes.empty())
    {
        processes.pop_back();
    }
    else
    {
        QMessageBox::warning(this, "Invalid Operation", "Nothing to be DELETED.");
        return;
    }

    // Get the model from the table view
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (model)
    {
        int rowCount = model->rowCount();
        if (rowCount > 0)
        {
            model->removeRow(rowCount - 1);
        }
    }
}

void clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (QWidget *widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
}

void MainWindow::on_restartButton_clicked()
{
    running_flag = false;
    finished = false;
    // Stoping timer
    timer->stop();
    // Enable startButton
    ui->startButton->setEnabled(true);

    // Reenable the radio buttons functionality
    on_schedulerSelect_currentTextChanged(ui->schedulerSelect->currentText());

    // Enable editing in table
    ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    // precesses reset ??
    while (!processes.empty())
    {
        processes.pop_back();
    }
    // table reset
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (model)
    {
        model->removeRows(0, model->rowCount());
    }
    // gantt chart clear
    clearLayout(ganttChart);
    GanttLastProcess = nullptr;
    GanttLastProcessFrame = nullptr;
    countLabel = nullptr;
    GanttLastSize = 0;
    timeCounter = 0;

    // schedular enable
    ui->schedulerSelect->setEnabled(true);
    // enable delete button
    ui->deleteButton->setEnabled(true);
    // clear avgTurnaroundText and avgWaitingText
    ui->avgTurnaroundText->clear();
    ui->avgWaitingText->clear();
}

void MainWindow::on_pauseButton_clicked()
{
    if (!finished)
    {

        if (ui->pauseButton->text() == "Pause")
        {
            timer->stop();
            ui->pauseButton->setText("Continue");
        }
        else
        {

            ui->pauseButton->setText("Pause");
            // Passing the new process to the backend
            // scheduler->addProcess();
            if (live)
            {
                timer->start(1000);
            }
            else
            {
                timer->start(10);
            }
        }
    }
}

void MainWindow::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    // For simplicity, assume only one cell was changed
    int row = topLeft.row();
    int column = topLeft.column();

    // Get the model
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (!model || row >= processes.size())
        return;

    // Update the corresponding process based on the changed column
    switch (column)
    {
    case 0: // Process Name
    {
        QString name = model->item(row, column)->text();
        processes[row]->setName(name.toStdString());
        // qDebug() << "Process name updated to:" << name;
    }
    break;
    case 2: // Remaining Time
    {
        int remaining = model->item(row, column)->text().toInt();
        processes[row]->setBurstTime(remaining);
        processes[row]->setRemainingTime(remaining);
        // qDebug() << "Process burst time updated to:" << remaining;
    }
    break;
    case 3: // Arrival Time
    {
        int arrival = model->item(row, column)->text().toInt();
        processes[row]->setArrivalTime(arrival);
        // qDebug() << "Process arrival time updated to:" << arrival;
    }
    break;
    }
}
