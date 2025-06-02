#pragma once


#include <QWidget>
#include <QLineEdit>

class ValueEdit: public QLineEdit{
    Q_OBJECT
private:
    bool correct;
    int minimum, maximum;

    void validator();
public:
    ValueEdit(QWidget *parent = nullptr);
    ValueEdit(int min, int max, QWidget *parent = nullptr);
    ValueEdit(int value, int min, int max, QWidget *parent = nullptr);

    int getValue();
    QString getNumberValue();
    void setValue(int value);
    void setRange(int min, int max);
    bool isCorrect();
};