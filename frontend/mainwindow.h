#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QTimer>
#include "toggle.h"
#include "process.h"
#include "Scheduler.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<std::shared_ptr<Process>> processes;

    void update_table_view(QTableView *tableView, const std::vector<std::shared_ptr<Process>> &processes);

private slots:
    // void on_Add_Button_clicked();
    void on_schedulerSelect_currentTextChanged(const QString &arg1);
    void onToggleSwitchStateChanged(bool checked);
    void on_addProcessButton_clicked();
    void on_startButton_clicked();
    void periodicFunction();

    void on_deleteButton_clicked();

    void on_restartButton_clicked();

    void on_pauseButton_clicked();

protected:
    void init_gui_elements();

private:
    void init_process_table(QTableView *tableView);
    void connect_signals();
    void updateGanttChart();
    void init_gantt_chart();
    void init_comboBox();
    void init_toggle_switch();
    void init_processor_image();
    void finalRunUpdate();
    Scheduler *startScheduler(const QString &selectedAlgorithm, bool isPreemptive);

    Ui::MainWindow *ui;
    ToggleSwitch *toggleSwitch;
    QTimer *timer;
    Scheduler *scheduler;
    bool live = true;
    bool finished = false;
    std::shared_ptr<Process> GanttLastProcess = nullptr;
    QFrame *GanttLastProcessFrame = nullptr;
    int GanttLastSize = 0;
};
#endif // MAINWINDOW_H
