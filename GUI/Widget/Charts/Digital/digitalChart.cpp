#include "digitalChart.hpp"


DigitalChart::DigitalChart(uint GPIO, QWidget *parent):
Chart("Logic", "GPIO: " + QString::number(GPIO), parent),
    gpio(GPIO)
{
}



void DigitalChart::setSeries(std::array<uint16_t, 65536> &data, qreal timeSpan){
    // if (data.isEmpty()) return;
    // series.clear();
    // for (uint i = 0; i < data.length()-1; i++){
    //     series.append(timeSpan*i, (data.at(i) >> gpio) & 1);
    //     series.append(timeSpan*i, (data.at(i+1) >> gpio) & 1);
    // }
    // series.append(timeSpan*(data.length()-1), (data.back() >> gpio) & 1);
}

void DigitalChart::setSeries(std::array<uint16_t, 65536> &data, std::array<uint16_t, 65536> &timeSpan){
    // series.clear();
    // for (uint i = 0; i < data.length()-1; i++){
    //     series.append(timeSpan.at(i), (data.at(i) >> gpio) & 1);
    //     series.append(timeSpan.at(i), (data.at(i+1) >> gpio) & 1);
    // }
    // series.append(timeSpan.at(data.length()-1), (data.back() >> gpio) & 1);
}

