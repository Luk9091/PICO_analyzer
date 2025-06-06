#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent),
    central(this),
    serial(this),
    chartUpdate_timer(this)
{
    setWindowTitle("Signal viewer");

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

    serial_ui->addSerialInstance(&serial);
    charts_scroll->setWidgetResizable(true);
    charts_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    charts_scroll->setWidget(charts_container);


    connect(topBar, &TopBar::addChart,      this, &MainWindow::openAddChatWindow);
    connect(topBar, &TopBar::sendConfig,    &serial, &Serial::writeInt);
    connect(topBar, &TopBar::runCaption,    this, &MainWindow::runCaption);
    // connect(charts_scroll->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::moveChartAxis);
    connect(&chartUpdate_timer, &QTimer::timeout, this, &MainWindow::updateChart);
    connect(&serial, &Serial::receivedData,  this, &MainWindow::storeNewData);

    chartUpdate_timer.setInterval(100);
}


void MainWindow::addChart(Chart *chart){
    bool pauseTimer = chartUpdate_timer.isActive();
    chartUpdate_timer.stop();

    chart->setParent(charts_scroll);
    connect(chart, &Chart::removeMe, this, [this, chart](){
        layout->removeWidget(chart);
        charts.removeOne(chart);
        chart->deleteLater();
    });
 
    charts_layout->addWidget(chart, 0, Qt::AlignTop);
    charts.append(chart);

    if (pauseTimer) chartUpdate_timer.start();
}

void MainWindow::runCaption(bool state, QVector<uint32_t> toSend){
    QVector<uint32_t> run;
    if (state){
        serial.clear();
        for (auto chart: charts){
            chart->clear();
        }
        data.clear();
        time.clear();

        serial.writeInt(toSend);
        run.append(CMD_DEVICE_RUN);

        chartUpdate_timer.start();
    } else {
        chartUpdate_timer.stop();
        run.append(CMD_DEVICE_STOP);
    }

    serial.writeInt(run);
}

void MainWindow::storeNewData(){
    QVector<uint16_t> read;
    int32_t tag = serial.read(read);

    if (data.length() >= topBar->getSampleLimit()){
        data.clear();
        time.clear();
    }

    switch (tag){
        case TAG_DIGITAL:{
            data.append(read);
        }break;
        case TAG_DIGITAL_TIMER:{
            for(uint i = 0; i < read.length(); i++){
                if (i % 2 == 0){
                    data.append(read.at(i));
                } else {
                    time.append(read.at(i));
                }
            }
        }break;

        
        default:
            break;
    }
}

void MainWindow::updateChart(){
    for (auto chart: charts){
        // if (time.size)
        if (time.length() == data.length() && time.length()){
            chart->setSeries(data, time);
        } else {
            chart->setSeries(data, 0.2);
        }
    }
}

void MainWindow::moveChartAxis(int scrollValue){
    for (auto chart: charts){
        chart->move(scrollValue);
    }
}


void MainWindow::openAddChatWindow(){
    if (addChartWindow_isOpen) return;
    
    addChartWindow_isOpen = true;
    addChartWindow = new AddChartWindow();
    
    connect(addChartWindow, &AddChartWindow::toClose, this, &MainWindow::onCloseAddChartWindow);
    connect(addChartWindow, &AddChartWindow::createSignalChart, this, &MainWindow::addChart);
}

void MainWindow::onCloseAddChartWindow(){
    addChartWindow_isOpen = false;
    delete addChartWindow;
    addChartWindow = nullptr;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if (addChartWindow){
        addChartWindow->close();
    }

    QWidget::closeEvent(event);
}


MainWindow::~MainWindow(){
    if (addChartWindow != nullptr){
        delete addChartWindow;
    }
}
