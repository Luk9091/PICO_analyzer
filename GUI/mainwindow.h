#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>

#include "serial.hpp"
#include "topBar.hpp"
#include "serial_ui.hpp"
#include "chart.hpp"
// #include "digitalChart.hpp"
#include "addChartWindow.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    TopBar *topBar;
    Serial_UI *serial_ui;
    
    QScrollArea *charts_scroll;
    QVBoxLayout *charts_layout;
    QWidget     *charts_container;
    QList<Chart*> charts;
    QTimer      chartUpdate_timer;
    

    QGridLayout *layout;
    QWidget central;

    Serial serial;
    
    bool addChartWindow_isOpen = false;
    AddChartWindow *addChartWindow = nullptr;

    QVector<uint16_t> data;
    QVector<uint16_t> time;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addChart(Chart *chart);
    void updateChart();
    void moveChartAxis(int scrollValue);

    void storeNewData();
    void runCaption();

    void openAddChatWindow();
    void onCloseAddChartWindow();
};
#endif // MAINWINDOW_H
