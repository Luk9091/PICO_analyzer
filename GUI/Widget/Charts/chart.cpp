#include "chart.hpp"


Chart::Chart(const QString& name_line, const QString& second_line, QWidget *parent):
QWidget(parent),
    layout(this),
    chart(),
    series(this),
    axisX(this),
    axisY(this)
{
    chart.addSeries(&series);
    chart.legend()->hide();
    axisX.setLabelsVisible(false);
    chart.setBackgroundVisible(false);
    chart.setMargins(QMargins(0, 0, 0, 0));

    axisX.setTickType(QValueAxis::TicksDynamic);
    axisX.setMinorTickCount(1);
    setRange(0, 256);

    axisY.hide();
    axisY.setMax(1.1);


    chart.addAxis(&axisX, Qt::AlignBottom);
    series.attachAxis(&axisX);
    chart.addAxis(&axisY, Qt::AlignLeft);
    series.attachAxis(&axisY);

    
    auto chartView = new QChartView(&chart, this);
    titleBox = placeDescription(name_line, second_line);

    layout.addWidget(titleBox, 0, 0, 1, 1);
    layout.addWidget(chartView, 0, 1, 2, 1);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &Chart::showContextMenu);
    setMinimumHeight(96);
    setMouseTracking(true);
}

Chart::Chart(const QString& name_line, const QString& second_line, bool analog, QWidget *parent):
Chart(name_line, second_line, parent)
{
    _analog = analog;
}

void Chart::showContextMenu(const QPoint &pos){
    QMenu contextMenu("Context menu", this);

    // QAction *renameAction = contextMenu.addAction("Rename");
    QAction *deleteAction = contextMenu.addAction("Remove");

    QAction *select = contextMenu.exec(mapToGlobal(pos));
    if (select == deleteAction){
        emit removeMe();
    }

}

bool Chart::isAnalog(){
    return _analog;
}

void Chart::setRange(qreal min, qreal max){
    axisX.setRange(min, max);
    axisX.setTickInterval((max - min)/10);

    int diff = max - min;

    if (length != diff){
        if (diff < length){
            for(int i = 0; i < length - diff; i++){
                series.remove(length - i);
            }
        } else{
            for(int i = length; i < diff; i++){
                series.append(length + i + min, 0);
            }
        }
        length = diff;
    }
}
void Chart::setMax(qreal value){
    axisX.setMax(value);
}
void Chart::setTickInterval(qreal span){
    axisX.setTickInterval(span);
}


void Chart::clear(){
    for (int i = 0; i < length; i++){
        series.replace(i, i, 0);
    }
}


void Chart::move(qreal value){
    axisX.setRange(axisX.min() + value, axisX.max() + value);
}

// void Chart::mouseMoveEvent(QMouseEvent *event){
//     // qDebug() << event->pos();
// }



QGroupBox *Chart::placeDescription(const QString& name_line, const QString& second_line){
    QGroupBox *box = new QGroupBox(this);
    QGridLayout *layout = new QGridLayout(box);

    QLabel *label = new QLabel(name_line, box);
    name = new QLabel(second_line, box);
    name->setAlignment(Qt::AlignRight);

    layout->addWidget(label);
    layout->addWidget(name);

    box->setFixedWidth(84);
    return box;
}