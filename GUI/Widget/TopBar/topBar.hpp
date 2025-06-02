#pragma once

#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
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
    ValueEdit   *preBuffer_edit;
    QComboBox   *trigMode_selector;
    QComboBox   *trigSrc_selector;
    QLineEdit   *sampleFreq_setter;
    QLineEdit   *basePosition_setter;

    int id;
public:
    TopBar(QWidget *patent = nullptr);

private:
    void place();

signals:
    void addChart(uint gpio, const QString& label);
    // void sendConfig(const QString& data);
    void sendConfig(const uint32_t data);
    void runCaption();
};