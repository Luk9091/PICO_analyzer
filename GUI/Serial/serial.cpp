#include "serial.hpp"

#include "communicationConfig.h"

QStringList getDevList(){
    QStringList list;
    const auto ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo& port: ports){
        const QString dev = port.systemLocation();
#if defined(Q_OS_LINUX)
        if (dev.startsWith("/dev/ttyS")) continue;

        list.append("/dev/" + port.portName());
#else
        list.append(port.portName());
#endif
    }

    return list;
}


Serial::Serial(QObject* parent):
Serial("", false, 115200, parent)
{

}

Serial::Serial(const QString& port, bool open, int baudrate, QObject* parent):
QObject(parent), device(this)
{
    device.setPortName(port);
    device.setBaudRate(baudrate);

    connect(&device, &QSerialPort::readyRead, this, &Serial::readyRead_handler);

    if (open){
        device.open(QIODevice::ReadWrite);
        setPort = true;
    } else {
        setPort = false;
    }

    busy = false;
}


Serial::~Serial(){
    if (isOpen()){
        device.close();
    }
}

bool Serial::isOpen(){
    if (!setPort) return false;
    return device.isOpen();
}

bool Serial::open(const QString& port){
    if (setPort){
        if (device.isOpen()) return false;
    }

    if (!port.isEmpty()){
        clear();
        device.setPortName(port);
        setPort = true;
        return device.open(QIODevice::ReadWrite);
    } 

    setPort = false;
    return false;
}

void Serial::close(){
    QVector<uint32_t> data;
    data.append(CMD_DEVICE_STOP);
    writeInt(data);

    device.close();
}

void Serial::clear(){
    release();
    writeQueue.clear();
    readQueue.clear();
}


bool Serial::isBusy(){
    return busy;
}

bool Serial::take(){
    if (isBusy()) return false;
    busy = true;
    return true;
}

bool Serial::release(){
    busy = false;
    return true;
}


int Serial::write(const QString& str){
    if (!isOpen()) return 0;
    const QByteArray message = str.toUtf8();
    if (!isBusy()){
        writeWithQueue();
    }
    return message.length();
    // return device.write(message);
}

int Serial::writeInt(const QVector<uint32_t>& data){
    if (!isOpen()) return 0;
    QByteArray message {};

    for (uint i = 0; i < data.length(); i++){
        toByte convert = {.u32 = data.at(i)};
        for (uint i = 0; i < 4; i++){
            message.append(convert.u8[3-i]);
        }
    }

    char EOL = 0;
    for (uint i = 0; i < 4; i++){
        message.append(EOL);
    }

    writeQueue.enqueue(message);

    if (!isBusy()){
        writeWithQueue();
    }
    return message.length();
    // return device.write(message);
}

int Serial::read(QString& str){
    if (readQueue.isEmpty()) return 0;

    QByteArray data = readQueue.dequeue();
    str = QString::fromUtf8(data);
    return str.length();
}

int Serial::read(QVector<uint16_t>& data){
    if (readQueue.isEmpty()) return 0;

    QByteArray byteArray = readQueue.dequeue();

    int32_t tag = 0;
    for (uint i = 0; i < byteArray.length(); i += 2){
        uint16_t value = byteArray.at(i+1) << 8 | byteArray.at(i) << 0;
        if (i < 2){
            tag |= value;
        } else if (i >= 4) {
            data.append(value);
        }
    }

    return tag;
}


bool Serial::readQueue_isEmpty(){
    return readQueue.isEmpty();
}

int Serial::readQueue_len(){
    return readQueue.length();
}

void Serial::readyRead_handler(){
    if (!device.isOpen()) return;
    QByteArray buffer;
    buffer = device.read(SERIAL_MSG_MAX_SIZE);
    int tag = buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];

    if (tag == TAG_OK){
        writeWithQueue();
    }
    else if (tag < TAG_OK){
        writeQueue.clear();
        release();
    }
    else if (tag > TAG_OK) {
        readQueue.enqueue(buffer);
        emit receivedData();
    }
}

void Serial::writeWithQueue(){
    if (writeQueue.isEmpty()){
        release();
        return;
    }

    take();
    auto message = writeQueue.dequeue();
    device.write(message);
}