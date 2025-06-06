#pragma once

#include <QVector>

#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include "valueEdit.hpp"

#include "communicationConfig.h"

class TopBar: public QWidget{
    Q_OBJECT
private:
    QPushButton *add_button;
    QPushButton *run_button;
    QGridLayout *layout;
    QComboBox   *mode_selector;
    ValueEdit   *buffer_edit;
    QCheckBox   *timerOn_checkBox;
    QComboBox   *triggerMode_selector;
    QComboBox   *triggerSource_selector;
    ValueEdit   *sampleFreq_setter;
    QLineEdit   *basePosition_setter;

    int id;
public:
    TopBar(QWidget *patent = nullptr);
    int getSampleLimit();
    bool isRun();

private:
    void place();
    QVector<uint32_t> sendMode();
    void onChangeTimeredMode(uint index);
    void onChangeTriggerList(uint index);
    void onRunClicked();

signals:
    void addChart(uint gpio, const QString& label);
    // void sendConfig(const QString& data);
    void sendConfig(const QVector<uint32_t> data);
    void runCaption(bool state, QVector<uint32_t> data);

};