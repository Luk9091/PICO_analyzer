#include "valueEdit.hpp"
#include <QValidator>



int convert2Int(const QString& input, bool *ok, int *base){
    int value;
    
    if (input.startsWith("0x")){
        QString str = input.sliced(2);
        value = str.toInt(ok, 16);
        if (base != nullptr) *base = 16;
    } else {
        value = input.toInt(ok, 10);
        if (base != nullptr) *base = 10;
    }
    if (*ok == false) return 0;
    return value;
}

void ValueEdit::validator(){
    bool ok;
    int base;

    int value = convert2Int(text(), &ok, &base);
    if (!ok){
        setText("NaN");
        correct = false;
        return;
    }


    correct = true;
    QString prefix = "";
    if (base == 16) prefix = "0x";
    else if (base == 2) prefix = "0x";
    
    if (value > maximum){
        setText(prefix + QString::number(maximum, base).toUpper());
    } else if (value < minimum){
        setText(prefix + QString::number(minimum, base).toUpper());
    }
}

ValueEdit::ValueEdit(QWidget *parent):
    QLineEdit(parent)
{
    correct = false;
    connect(this, &QLineEdit::editingFinished, this, &ValueEdit::validator);
}

ValueEdit::ValueEdit(int min, int max, QWidget *parent):
    ValueEdit(parent)
{
    setRange(min, max);
}
ValueEdit::ValueEdit(int value, int min, int max, QWidget *parent):
    ValueEdit(min, max, parent)
{
    setValue(value);
}

int ValueEdit::getValue(){
    bool ok;
    int base;
    int value = convert2Int(text(), &ok, &base);
    return value;
}

QString ValueEdit::getNumberValue(){
    int value = getValue();
    return QString::number(value);
}

void ValueEdit::setValue(int value){
    
    setText(QString::number(value));
}

void ValueEdit::setRange(int min, int max){
    minimum = min;
    maximum = max;
}

bool ValueEdit::isCorrect(){
    return correct;
}