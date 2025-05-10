#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

#include "serial.hpp"
#include "topBar.hpp"
#include "serial_ui.hpp"
#include "digitalChart.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    TopBar *topBar;
    Serial_UI *serial_ui;
    
    QScrollArea *charts_scroll;
    QVBoxLayout *charts_layout;
    QWidget     *charts_container;
    QList<DigitalChart*> charts;
    

    QGridLayout *layout;
    QWidget central;

    Serial serial;
    

    QVector<uint16_t> data;
    QVector<uint16_t> timeStamp;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void addChart(uint gpio, const QString& label);
    void removeChart(uint index);
    void updateChart();
    void moveChartAxis(int scrollValue);
};
#endif // MAINWINDOW_H
