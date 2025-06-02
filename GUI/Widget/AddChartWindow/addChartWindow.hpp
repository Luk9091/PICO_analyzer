#pragma once

#include <QVector>

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>

#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>

#include "chart.hpp"
#include "digitalChart.hpp"
#include "analogChart.hpp"

enum ChartType{
    DIGITAL,
    ANALOG,
};


class AddChartWindow: public QWidget{
    Q_OBJECT
private:
    QGridLayout *layout;
    QComboBox   *signalSelector;

    QWidget     *signal_container;
    QVBoxLayout *signal_layout;
    QScrollArea *signal_area;
    QVector<QCheckBox*> signals_vect;

    QPushButton *accept_button;


public:
    AddChartWindow(QWidget *parent = nullptr);

private slots:
    void clearChannel();
    void addDigitalChannel();
    void addAnalogChannel();

    void changeDisplayChannel(int index);

    void onAccept();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void createSignalChart(Chart *newChart);
    void toClose();
};