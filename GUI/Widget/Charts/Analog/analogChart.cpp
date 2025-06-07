#include "analogChart.hpp"


AnalogChart::AnalogChart(int channel, QWidget *parent):
Chart("Analog", "Chan: " + QString::number(channel), true, parent),
    channel(channel)
{
    titleBox->setMinimumHeight(minimumHeight() * 1.25);
    setMinimumHeight(minimumHeight() * 1.5);
}