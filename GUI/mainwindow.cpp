#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent),
    central(this),
    serial(this)
{
    for (uint i = 0; i < 128; i++){
        data.append(i);
    }

    setMinimumSize(896, 600);
    setCentralWidget(&central);
    layout = new QGridLayout(&central);

    topBar          = new TopBar(&central);
    serial_ui       = new Serial_UI(&central);
    charts_container= new QWidget(&central);
    charts_scroll   = new QScrollArea(&central);
    charts_layout   = new QVBoxLayout(charts_container);



    layout->addWidget(topBar,       0, 0, 1, 4, Qt::AlignTop);
    layout->addWidget(serial_ui,    0, 4, 1, 1, Qt::AlignTop);
    layout->addWidget(charts_scroll,1, 0, 1, 5);

    charts_scroll->setWidgetResizable(true);
    charts_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    charts_scroll->setWidget(charts_container);

    connect(topBar, &TopBar::addChart,      this, &MainWindow::addChart);
    connect(topBar, &TopBar::runCaption,    this, &MainWindow::updateChart);
    connect(charts_scroll->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::moveChartAxis);
}


void MainWindow::addChart(uint gpio, const QString& label){
    DigitalChart *newChart = new DigitalChart(gpio, charts_scroll);

    charts_layout->addWidget(newChart, 0, Qt::AlignTop);
    charts.append(newChart);
}

void MainWindow::removeChart(uint index){

}

void MainWindow::updateChart(){
    for (auto chart: charts){
        if (timeStamp.length() == data.length() && timeStamp.length()){
            chart->setSeries(data, timeStamp);
        } else {
            chart->setSeries(data, 1);
        }
    }
}

void MainWindow::moveChartAxis(int scrollValue){
    for (auto chart: charts){
        chart->move(scrollValue);
    }
}

MainWindow::~MainWindow(){
}
