#pragma once

#include "chart.hpp"

class DigitalChart: public Chart{
    Q_OBJECT
private:
    uint gpio;
public:
    DigitalChart(uint GPIO, QWidget *parent = nullptr);

    // void setSeries      (std::span<uint16_t> data, qreal timeSpan) override;
    void setSeries      (std::span<uint16_t> data, std::span<uint16_t> timeSpan) override;
};