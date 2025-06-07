#pragma once


#include <QWidget>
#include <QLineEdit>

class ValueEdit: public QLineEdit{
    Q_OBJECT
private:
    bool correct;
    int storeValue;
    int minimum, maximum;
    uint step;

    void validator();
    int roundToStep(int value);
public:
    ValueEdit(QWidget *parent = nullptr);
    ValueEdit(int min, int max, QWidget *parent = nullptr);
    ValueEdit(int value, int min, int max, QWidget *parent = nullptr);
    ValueEdit(int value, int min, int max, int step, QWidget *parent = nullptr);

    int getValue();
    QString getNumberValue();
    void setValue(int value);
    void setRange(int min, int max);
    void setStep(uint value = 1);
    bool isCorrect();

signals:
    void validData();
};