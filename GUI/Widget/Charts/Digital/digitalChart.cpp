#include "digitalChart.hpp"


DigitalChart::DigitalChart(uint GPIO, QWidget *parent):
Chart("Logic", "GPIO: " + QString::number(GPIO), parent),
    gpio(GPIO)
{
}



void DigitalChart::setSeries(std::span<uint16_t> data, std::span<uint16_t> timeSpan){
    uint data_size = data.size();
    if (data_size == 0) return;
    uint bit;
    for (uint i = 0; i < data_size; i++){
        bit = (data[i] >> gpio) & 1;
        series.replace(i, timeSpan[i], bit);
    }
}

