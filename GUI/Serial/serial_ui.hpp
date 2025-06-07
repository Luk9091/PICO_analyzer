#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QBoxLayout>

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>


#include "serial.hpp"

class Serial_ComboBox: public QComboBox{
    Q_OBJECT
public:
    // using QComboBox::QComboBox;

    Serial_ComboBox(QWidget *parent = nullptr);

protected:
    void showPopup() override;

private:
    void updateDeviceList();

signals:
    void lostPort();
};


class Serial_UI: public QWidget{
    Q_OBJECT
private:

    QGridLayout         layout;
    QLabel              label;
    QPushButton         connectionButton;
    Serial_ComboBox     deviceSelector;

    QPointer<Serial> serial;

public:
    Serial_UI(QWidget *parent = nullptr);
    Serial_UI(const QString& serialDevName, QWidget *parent = nullptr);
    void addSerialInstance(QPointer<Serial> sharedSerial);
    // ~Serial_UI();

private:


private slots:
    void portOpen();
    void portClose();
};