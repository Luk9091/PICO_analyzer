#pragma once

#include <QObject>
#include <QtSerialPort>
#include <QQueue>
#include <QStringList>


#define SERIAL_MSG_MAX_SIZE 256

QStringList getDevList();




class Serial: public QObject{
    Q_OBJECT
private:
    QSerialPort device;
    QQueue<QByteArray> readQueue;

    bool busy;
public:
    Serial(QObject *parent = nullptr);
    Serial(const QString& port, bool open = false, int baudrate = 115200, QObject *parent = nullptr);
    ~Serial();

    bool isOpen();
    bool open(const QString& port = "");
    void close();

    bool isBusy();
    bool take();
    bool release();

    int write(const QString& str);
    int read(QString& str);
    // int readLine(QString& out);
    // int readUntil(void *data, int size = 1);

    bool readQueue_isEmpty();
    int  readQueue_len();

signals:
    void lineReceive(const QString& line);

private slots:
    void readyRead_handler();
};