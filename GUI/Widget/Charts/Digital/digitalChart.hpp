#pragma once

#include <QVector>

#include <QWidget>

#include <QGridLayout>

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>


class DigitalChart: public QWidget{
    Q_OBJECT
private:
    QGridLayout layout;

    QChart      chart;
    QLineSeries series;
    QValueAxis  axisX;
    QValueAxis  axisY;

    QLabel      *name;

    uint gpio;
public:
    DigitalChart(uint GPIO, QWidget *parent = nullptr);
    
    void setRange       (qreal min, qreal max);
    void setMax         (qreal value);
    void setTickInterval(qreal span);

    void setSeries      (QVector<uint16_t> &data, qreal timeSpan);
    void setSeries      (QVector<uint16_t> &data, QVector<uint16_t> timeSpan);

    void move           (qreal value);

private:
    void mouseMoveEvent(QMouseEvent *event);


    QGroupBox *placeDescription();
};