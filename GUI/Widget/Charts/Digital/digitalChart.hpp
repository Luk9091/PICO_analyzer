#pragma once

#include "chart.hpp"

class DigitalChart: public Chart{
    Q_OBJECT
private:
    uint gpio;
public:
    DigitalChart(uint GPIO, QWidget *parent = nullptr);

    void setSeries      (std::array<uint16_t, 65536> &data, qreal timeSpan) override;
    void setSeries      (std::array<uint16_t, 65536> &data, std::array<uint16_t, 65536> &timeSpan) override;
};