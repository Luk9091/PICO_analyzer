#include "digitalChart.hpp"


DigitalChart::DigitalChart(uint GPIO, QWidget *parent):
QWidget(parent),
    layout(this),
    chart(),
    series(this),
    axisX(this),
    axisY(this),
    gpio(GPIO)
{
    chart.addSeries(&series);
    chart.legend()->hide();
    axisX.setLabelsVisible(false);
    chart.setBackgroundVisible(false);
    chart.setMargins(QMargins(0, 0, 0, 0));

    axisX.setTickType(QValueAxis::TicksDynamic);
    axisX.setTickInterval(10);
    axisX.setMinorTickCount(1);
    setRange(0, 100);

    axisY.hide();
    axisY.setMax(1.1);


    chart.addAxis(&axisX, Qt::AlignBottom);
    series.attachAxis(&axisX);
    chart.addAxis(&axisY, Qt::AlignLeft);
    series.attachAxis(&axisY);

    
    auto chartView = new QChartView(&chart, this);

    auto titleBox = placeDescription();


    layout.addWidget(titleBox, 0, 0, 1, 1);
    layout.addWidget(chartView, 0, 1, 2, 1);



    setMinimumHeight(96);
    setMaximumHeight(96);
    setMouseTracking(true);
}


void DigitalChart::setRange(qreal min, qreal max){
    axisX.setRange(min, max+1);
}
void DigitalChart::setMax(qreal value){
    axisX.setMax(value);
}
void DigitalChart::setTickInterval(qreal span){
    axisX.setTickInterval(span);
}

void DigitalChart::setSeries(QVector<uint16_t> &data, qreal timeSpan){
    series.clear();
    for (uint i = 0; i < data.length()-1; i++){
        series.append(timeSpan*i, (data.at(i) >> gpio) & 1);
        series.append(timeSpan*i, (data.at(i+1) >> gpio) & 1);
    }
    series.append(timeSpan*(data.length()-1), (data.back() >> gpio) & 1);
}

void DigitalChart::setSeries(QVector<uint16_t> &data, QVector<uint16_t> timeSpan){
    series.clear();
    for (uint i = 0; i < data.length()-1; i++){
        series.append(timeSpan.at(i), (data.at(i) >> gpio) & 1);
        series.append(timeSpan.at(i), (data.at(i+1) >> gpio) & 1);
    }
    series.append(timeSpan.at(data.length()-1), (data.back() >> gpio) & 1);
}

void DigitalChart::move(qreal value){
    axisX.setRange(axisX.min() + value, axisX.max() + value);
}

void DigitalChart::mouseMoveEvent(QMouseEvent *event){
    qDebug() << event->pos();
}

QGroupBox *DigitalChart::placeDescription(){
    QGroupBox *box = new QGroupBox(this);
    QGridLayout *layout = new QGridLayout(box);

    QLabel *label = new QLabel("Logic", box);
    name = new QLabel("GPIO " + QString::number(gpio), box);
    name->setAlignment(Qt::AlignRight);

    layout->addWidget(label);
    layout->addWidget(name);

    return box;
}
