#pragma once

#include "chart.hpp"

class DigitalChart: public Chart{
    Q_OBJECT
private:
    uint gpio;
public:
    DigitalChart(uint GPIO, QWidget *parent = nullptr);

    void setSeries      (QVector<uint16_t> &data, qreal timeSpan) override;
    void setSeries      (QVector<uint16_t> &data, QVector<uint16_t> timeSpan) override;
};