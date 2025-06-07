#include "addChartWindow.hpp"




AddChartWindow::AddChartWindow(QWidget *parent):
QWidget(parent)
{
    setWindowTitle("Add signal");
    layout = new QGridLayout(this);
    signalSelector = new QComboBox(this);

    accept_button = new QPushButton("Accept", this);

    signal_area         = new QScrollArea(this);
    signal_container    = new QWidget(this);
    signal_layout       = new QVBoxLayout(signal_container);


    signalSelector->addItem("Digital");
    signalSelector->addItem("Analog");

    addDigitalChannel();

    signal_area->setWidgetResizable(true);
    signal_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    signal_area->setWidget(signal_container);


    layout->addWidget(signalSelector, 0, 0, 1, 3);
    layout->addWidget(signal_area,    1, 0, 2, 3);
    layout->addWidget(accept_button,  3, 2, 1, 1);
    

    connect(signalSelector, &QComboBox::currentIndexChanged, this, &AddChartWindow::changeDisplayChannel);
    connect(accept_button, &QPushButton::clicked, this, &AddChartWindow::onAccept);

    resize(300, 320);
    setMinimumSize(300, 320);
    
    show();
}

void AddChartWindow::clearChannel(){
    for (auto channel : signals_vect){
        delete channel;
    }
    signals_vect.clear();
}

void AddChartWindow::addDigitalChannel(){
    for (uint i = 0; i < 16; i++){
        auto *signal = new QCheckBox("GPIO " + QString::number(i), this);
        signal_layout->addWidget(signal, 0, Qt::AlignTop);
        signals_vect.append(signal);
    }
}

void AddChartWindow::addAnalogChannel(){
    for (uint i = 0; i < 4; i++){
        auto *signal = new QCheckBox("Analog " + QString::number(i), this);
        signal_layout->addWidget(signal, 0, Qt::AlignTop);
        signals_vect.append(signal);
    }
    signal_layout->addStretch(0);
}



void AddChartWindow::changeDisplayChannel(int index){
    clearChannel();
    switch (index){
        case 0: addDigitalChannel(); break;
        case 1: addAnalogChannel(); break;
    }
}

void AddChartWindow::onAccept(){
    int index = signalSelector->currentIndex();
    for (uint i = 0; i < signals_vect.length(); i++){
        if (!signals_vect[i]->isChecked()) continue;

        switch (index){
            case 0: {
                Chart *chart = new DigitalChart(i);
                emit createSignalChart(chart);
            } break;

            case 1:{
                Chart *chart = new AnalogChart(i);
                emit createSignalChart(chart);
            } break;

            default:return;
        }
    }

    close();
}

void AddChartWindow::closeEvent(QCloseEvent *event) {
    emit toClose();
    QWidget::closeEvent(event);
}