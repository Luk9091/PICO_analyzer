#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVector>
#include <array>

#include "array2d.hpp"

#include "serial.hpp"
#include "topBar.hpp"
#include "serial_ui.hpp"
#include "chart.hpp"
#include "digitalChart.hpp"
#include "addChartWindow.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    TopBar          *topBar;
    Serial_UI       *serial_ui;
    
    QScrollArea     *charts_scroll;
    QVBoxLayout     *charts_layout;
    QWidget         *charts_container;
    QList<Chart*>   charts;
    // QTimer          chartUpdate_timer;
    

    QGridLayout *layout;
    QWidget central;

    Serial serial;
    
    bool                addChartWindow_isOpen = false;
    AddChartWindow      *addChartWindow = nullptr;

    Array2D<uint16_t, 65536> data;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addChart(Chart *chart);
    void updateChart();
    void moveChartAxis(int scrollValue);
    void resizeCharts();

    void storeNewData();
    void runCaption(bool state, QVector<uint32_t> data);

    void openAddChatWindow();
    void onCloseAddChartWindow();
};
#endif // MAINWINDOW_H
