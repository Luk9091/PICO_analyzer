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

    QString getNumberValue();
    void setRange(int min, int max);
    bool isCorrect();
};