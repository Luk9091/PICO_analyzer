#pragma once

#include "chart.hpp"

class AnalogChart: public Chart{
    Q_OBJECT
private:
    int channel;

    QLabel *voltageRange;

public:
    AnalogChart(int channel, QWidget *parent = nullptr);

    // void setSeries      (QVector<double> &data, qreal timeSpan) override;
    // void setSeries      (QVector<double> &data, QVector<uint16_t> timeSpan) override;
};