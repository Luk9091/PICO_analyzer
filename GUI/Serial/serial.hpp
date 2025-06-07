#pragma once

#include <QObject>
#include <QtSerialPort>
#include <QQueue>
#include <QStringList>


#define SERIAL_MSG_MAX_SIZE 64 

QStringList getDevList();


typedef union {
    uint32_t u32;
    char  u8[4];
} toByte;


class Serial: public QObject{
    Q_OBJECT
private:
    QSerialPort device;
    QQueue<QByteArray> readQueue;
    QQueue<QByteArray> writeQueue;
    bool setPort;

    bool busy;
    void writeWithQueue();
public:
    Serial(QObject *parent = nullptr);
    Serial(const QString& port, bool open = false, int baudrate = 115200, QObject *parent = nullptr);
    ~Serial();

    bool isOpen();
    bool open(const QString& port = "");
    void close();
    void clear();

    bool isBusy();
    bool take();
    bool release();

    int write(const QString& str);
    int writeInt(const QVector<uint32_t>& data);
    int read(QString& str);
    int read(QVector<uint16_t>& data);


    bool readQueue_isEmpty();
    int  readQueue_len();

signals:
    void receivedData();


private slots:
    void readyRead_handler();
};