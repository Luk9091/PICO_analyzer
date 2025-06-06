#pragma once


#include <QVector>
#include <span>

#include <QWidget>
#include <QMenu>

#include <QGridLayout>

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>

class Chart: public QWidget{
    Q_OBJECT
private:
    bool _analog;
protected:
    QGridLayout layout;

    QChart      chart;
    QLineSeries series;
    QValueAxis  axisX;
    QValueAxis  axisY;

    QGroupBox   *titleBox;

    QLabel      *name;
    uint        length = 0;

public:
    Chart(const QString& name_line, const QString& second_line, bool analog, QWidget *parent = nullptr);
    Chart(const QString& name_line, const QString& second_line, QWidget *parent = nullptr);
    
    bool isAnalog();

    void setRange       (qreal min, qreal max);
    void setMax         (qreal value);
    void setTickInterval(qreal span);

    virtual void setSeries      (std::span<uint16_t> data, qreal timeSpan) {};
    virtual void setSeries      (std::span<uint16_t> data, std::span<uint16_t> timeSpan) {};
    virtual void setSeries      (QVector<uint16_t> &data, qreal timeSpan) {};
    virtual void setSeries      (QVector<uint16_t> &data, QVector<uint16_t> timeSpan) {};

    void clear          ();
    void move           (qreal value);

protected:
    // void mouseMoveEvent(QMouseEvent *event);


    QGroupBox *placeDescription(const QString& name_line, const QString& second_line);
private slots:
    void showContextMenu(const QPoint &pos);


signals:
    void removeMe();

};