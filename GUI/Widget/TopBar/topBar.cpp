#include "topBar.hpp"



TopBar::TopBar(QWidget *parent):
QWidget(parent),
    id(0)
{
    layout = new QGridLayout(this);
    place();
    setMinimumWidth(720);
}



void TopBar::place(){
    add_button = new QPushButton("Add", this);
    run_button = new QPushButton("Run", this);
    add_button->setMinimumHeight(run_button->height()*2);
    run_button->setMinimumHeight(run_button->height()*2);

    QLabel *mode_label = new QLabel("Mode:", this);
    QLabel *buffer_label = new QLabel("Buffer:", this);
    mode_selector = new QComboBox(this);
    preBuffer_edit = new QLineEdit(this);
    buffer_edit = new QLineEdit(this);

    QLabel *trigMode_label = new QLabel("Trigger:", this);
    QLabel *trigSrc_label  = new QLabel("Source:", this);
    trigMode_selector = new QComboBox(this);
    trigSrc_selector = new QComboBox(this);

    QLabel *sampleFreq_label = new QLabel("Sample freq:", this);
    QLabel *basePosition_label = new QLabel("Base position:", this);
    sampleFreq_setter = new QLineEdit(this);
    basePosition_setter = new QLineEdit(this);


    layout->addWidget(add_button,           0, 0, 2, 1);
    layout->addWidget(run_button,           0, 1, 2, 1);

    layout->addWidget(mode_label,           0, 2, 1, 1);
    layout->addWidget(buffer_label,         1, 2, 1, 1);
    layout->addWidget(mode_selector,        0, 3, 1, 2);
    layout->addWidget(preBuffer_edit,       1, 3, 1, 1);
    layout->addWidget(buffer_edit,          1, 4, 1, 1);

    layout->addWidget(trigMode_label,       0, 5, 1, 1);
    layout->addWidget(trigSrc_label,        1, 5, 1, 1);
    layout->addWidget(trigMode_selector,    0, 6, 1, 1);
    layout->addWidget(trigSrc_selector,     1, 6, 1, 1);

    layout->addWidget(sampleFreq_label,     0, 7, 1, 1);
    layout->addWidget(basePosition_label,   1, 7, 1, 1);
    layout->addWidget(sampleFreq_setter,    0, 8, 1, 1);
    layout->addWidget(basePosition_setter,  1, 8, 1, 1);

    mode_selector->setFixedWidth(152);
    preBuffer_edit->setFixedWidth(72);
    buffer_edit->setFixedWidth(72);

    QString name = "Created";
    connect(add_button, &QPushButton::clicked, this, [this](){
        addChart(id, "Created");
        id++;
    });

    connect(run_button, &QPushButton::clicked, this, [this](){
        emit runCaption();
    });
}