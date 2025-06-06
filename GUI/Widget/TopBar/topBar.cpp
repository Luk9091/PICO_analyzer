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
    timerOn_checkBox = new QCheckBox("Timer", this);
    buffer_edit     = new ValueEdit(16384, 1, 65'536, this);

    QLabel *trigMode_label = new QLabel("Trigger:", this);
    QLabel *trigSrc_label  = new QLabel("Source:", this);
    triggerMode_selector = new QComboBox(this);
    triggerSource_selector = new QComboBox(this);

    QLabel *sampleFreq_label = new QLabel("Sample freq:", this);
    QLabel *basePosition_label = new QLabel("Base position:", this);
    sampleFreq_setter = new ValueEdit(5'000, 5'000, 200'000'000, 1'000, this);
    basePosition_setter = new QLineEdit(this);


    layout->addWidget(add_button,               0, 0, 2, 1);
    layout->addWidget(run_button,               0, 1, 2, 1);

    layout->addWidget(mode_label,               0, 2, 1, 1);
    layout->addWidget(buffer_label,             1, 2, 1, 1);
    layout->addWidget(mode_selector,            0, 3, 1, 2);
    layout->addWidget(buffer_edit,              1, 3, 1, 1);
    layout->addWidget(timerOn_checkBox,         1, 4, 1, 1);

    layout->addWidget(trigMode_label,           0, 5, 1, 1);
    layout->addWidget(trigSrc_label,            1, 5, 1, 1);
    layout->addWidget(triggerMode_selector,     0, 6, 1, 1);
    layout->addWidget(triggerSource_selector,   1, 6, 1, 1);

    layout->addWidget(sampleFreq_label,         0, 7, 1, 1);
    layout->addWidget(basePosition_label,       1, 7, 1, 1);
    layout->addWidget(sampleFreq_setter,        0, 8, 1, 1);
    layout->addWidget(basePosition_setter,      1, 8, 1, 1);

    mode_selector->setFixedWidth(152);
    timerOn_checkBox->setFixedWidth(72);
    buffer_edit->setFixedWidth(72);
    run_button->setCheckable(true);

    QString name = "Created";
    connect(add_button, &QPushButton::clicked, this, [this](){
        addChart(id, "Created");
        id++;
    });

    connect(run_button, &QPushButton::clicked, this, &TopBar::onRunClicked);


    mode_selector->addItem("Free run");
    // mode_selector->addItem("Triggered");
    mode_selector->addItem("Counted");
    // connect(mode_selector, &QComboBox::currentIndexChanged, this, &TopBar::sendMode);


    triggerSource_selector->addItem("Trigger");
    triggerSource_selector->addItem("GPIO");
    triggerSource_selector->addItem("Internal");
    onChangeTriggerList(triggerSource_selector->currentIndex());
    connect(triggerSource_selector, &QComboBox::currentIndexChanged, this, &TopBar::onChangeTriggerList);
}



QVector<uint32_t> TopBar::sendMode(){
        QVector<uint32_t> data;

        if (mode_selector->currentIndex() == 1){
            data.append(CMD_DIGITAL_MODE_COUNT);
        } else {
            if (triggerSource_selector->currentText() != "Internal"){
                data.append(CMD_DIGITAL_MODE_TRIGGERED);
            } else {
                data.append(CMD_DIGITAL_MODE_FREERUN);
            }
        }

        if (timerOn_checkBox->isChecked() && timerOn_checkBox->isEnabled()){ 
            // Timer on
            data.append(sampleFreq_setter->getValue());
        } else {                            // Time off
            data.append(0);
        }

        uint triggerPin = triggerSource_selector->currentIndex();
        if (triggerMode_selector->currentIndex() == 0) {
            triggerPin += 16;
        }
        data.append(triggerPin);

        // emit sendConfig(data);
        return data;
}


void TopBar::onChangeTriggerList(uint index){
    triggerMode_selector->clear();
    // if (mode_selector->currentIndex() != 0)
    timerOn_checkBox->setEnabled(true);
    triggerMode_selector->setEnabled(true);

    switch (index){
    case 0:{
        for (uint i = 0; i < 2; i++){
            triggerMode_selector->addItem("Trigger " + QString::number(i));
        }
    } break;
    case 1:{
        for (uint i = 0;  i < 16; i++){
            triggerMode_selector->addItem("GPIO " + QString::number(i));
        }
    }break;
    case 2:{
        triggerMode_selector->addItem("Auto");
        timerOn_checkBox->setEnabled(false);
        triggerMode_selector->setEnabled(false);
    }break;
    
    default:
        break;
    }

}



void TopBar::onRunClicked(){
    QVector<uint32_t> data;;
    bool state = run_button->isChecked();
    if (state){
        data = sendMode();
    }

    emit runCaption(state, data);
}


int TopBar::getSampleLimit(){
    return buffer_edit->getValue();
}

bool TopBar::isRun(){
    return run_button->isChecked();
}