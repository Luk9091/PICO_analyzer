#include "serial.hpp"
#include <QDebug>

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
        device.setPortName(port);
        setPort = true;
        return device.open(QIODevice::ReadWrite);
    } 

    setPort = false;
    return false;
}

void Serial::close(){
    device.close();
    busy = false;
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
    int status = device.write(message);
    return status;
}

int Serial::writeInt(const uint32_t data){
    if (!isOpen()) return 0;
    QByteArray message {};
    toByte convert = {.u32 = data};
    for (uint i = 0; i < 4; i++){
        message.append(convert.u8[3-i]);
    }
    return device.write(message);
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
    QDataStream stream(byteArray);

    while (!stream.atEnd()){
        quint16 value;
        stream >> value;
        data.append(value);
    }

    return byteArray.length()/2;
}


bool Serial::readQueue_isEmpty(){
    return readQueue.isEmpty();
}

int Serial::readQueue_len(){
    return readQueue.length();
}

void Serial::readyRead_handler(){
    QByteArray buffer;
    buffer = device.read(SERIAL_MSG_MAX_SIZE);

    readQueue.enqueue(buffer);
    emit receivedData();
}