#include "serial_ui.hpp"



Serial_UI::Serial_UI(QWidget *parent):
Serial_UI("Serial:", parent)
{

}
Serial_UI::Serial_UI(const QString& serialDevName, QWidget *parent):
QWidget(parent),
    layout(this),
    label(serialDevName, this),
    connectionButton("Connect", this),
    deviceSelector(this)
{

    layout.addWidget(&label,            0, 0);
    layout.addWidget(&deviceSelector,   0, 1);
    layout.addWidget(&connectionButton, 1, 0, 1, 2);

    connectionButton.setCheckable(true);
    connect(&connectionButton, &QPushButton::clicked, this, &Serial_UI::portOpen);
    connect(&deviceSelector, &Serial_ComboBox::lostPort, this, &Serial_UI::portClose);
}

void Serial_UI::addSerialInstance(QPointer<Serial> sharedSerial){
    serial = sharedSerial;
}

void Serial_UI::portOpen(){
    if(serial->open(deviceSelector.currentText())){
        connectionButton.setText("Disconnect");
    } else {
        portClose();
    }

}

void Serial_UI::portClose(){
    if (serial->isOpen()) serial->close();
    connectionButton.setText("Connect");
    connectionButton.setChecked(false);
}



Serial_ComboBox::Serial_ComboBox(QWidget *parent):
QComboBox(parent){
    updateDeviceList();
}

void Serial_ComboBox::showPopup(){
    updateDeviceList();
    QComboBox::showPopup();
}

void Serial_ComboBox::updateDeviceList(){
    auto current = currentText();

    clear();

    auto devList = getDevList();
    addItems(devList);
    addItem("/dev/pts/6");
    if (!devList.contains(current)){
        emit lostPort();
    }
}

