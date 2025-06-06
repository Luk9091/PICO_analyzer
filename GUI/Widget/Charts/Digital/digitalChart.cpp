#include "digitalChart.hpp"


DigitalChart::DigitalChart(uint GPIO, QWidget *parent):
Chart("Logic", "GPIO: " + QString::number(GPIO), parent),
    gpio(GPIO)
{
}



void DigitalChart::setSeries(std::span<uint16_t> data, qreal timeSpan){
    series.clear();
    for (uint i = 0; i < data.size()-1; i++){
        series.append(timeSpan*i, (data[i] >> gpio) & 1);
        series.append(timeSpan*i, (data[i+1] >> gpio) & 1);
    }
    series.append(timeSpan*(data.size()-1), (data.back() >> gpio) & 1);
}

void DigitalChart::setSeries(std::span<uint16_t> data, std::span<uint16_t> timeSpan){
    series.clear();
    for (uint i = 0; i < data.size()-1; i++){
        series.append(timeSpan[i], (data[i] >> gpio) & 1);
        series.append(timeSpan[i], (data[i+1] >> gpio) & 1);
    }
    series.append(timeSpan.back(), (data.back() >> gpio) & 1);
}

