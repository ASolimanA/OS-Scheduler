#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "toggle.h"

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

private slots:
    // void on_Add_Button_clicked();
    // void on_Start_Button_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void onToggleSwitchStateChanged(bool checked);
    void on_addProcessButton_clicked();

protected:
    void init_gui_elements();

private:
    void init_process_table(QTableView *tableView);
    void connect_signals();
    void init_gantt_chart();
    void init_comboBox();
    void init_toggle_switch();

    Ui::MainWindow *ui;
    ToggleSwitch *toggleSwitch;
};
#endif // MAINWINDOW_H
