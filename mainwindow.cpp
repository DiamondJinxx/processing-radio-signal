#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sPerpen = new QLineSeries();
    sParallel = new QLineSeries();
    sParWithOutNull = new QLineSeries();
    sPerWithOutNull = new QLineSeries();
    sParWithOutNosie = new QLineSeries();
    sPerWithOutNosie = new QLineSeries();
    sParWithOutNosieInv = new QLineSeries();
    sPerWithOutNosieInv = new QLineSeries();
    sParNorm = new QLineSeries();
    sPerNorm = new QLineSeries();
    sParNorm12 = new QLineSeries();
    sPerNorm12 = new QLineSeries();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnGo_clicked()
{
    begin();
    dellZero();
    delNoise();
    invert();
    normValue();
    normKiloMetrs();
}


void MainWindow::begin()
{
    QFile file("2017-04-11_-_14-06-06_4_raw.csv");
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
    {
        qDebug() << "Data file not exists";
        QMessageBox::warning(this,"Waring","Data file  not exist");
    }
    else {
        // Создаём поток для извлечения данных из файла
        QTextStream in(&file);

        QChartView *chartView = new QChartView(this);

        // Считываем данные до конца файла
        int row = 0; // строки в файле, для оси X
        while (!in.atEnd())
        {
            // ... построчно
            QString data = in.readLine();
            QStringList lstData = data.split(';');

            //unsigned int dPar = lstData.at(0).toInt() - lstNoise.at(0).toInt();
            //unsigned int dPer = lstData.at(1).toInt() - lstNoise.at(1).toInt();

            sParallel->append(row,lstData.at(0).toInt());
            sPerpen->append(row,lstData.at(1).toInt());
            sParWithOutNull->append(row,lstData.at(0).toInt());
            sPerWithOutNull->append(row,lstData.at(1).toInt());
            //vect1.append(lstData.at(0).toInt());
            //vect2.append(lstData.at(1).toInt());

            row++;
        }

        file.close();
        // Создаётся график и добавляется в него серия значений
        addChart(sParallel,sPerpen,"Начальные значения");

    }
}

void MainWindow::dellZero()
{
    this->minIndexPar = findMinIndex(this->sParallel);
    this->minIndexPer = findMinIndex(this->sPerpen);
    this->sParWithOutNull->removePoints(0,this->minIndexPar - 1);
    this->sPerWithOutNull->removePoints(0,this->minIndexPer - 1);
    addChart(this->sParWithOutNull, this->sPerWithOutNull, "Убрали нулевую дорожку");
}

void MainWindow::delNoise()
{
        QFile noise("noise.csv");
        QTextStream inNoise(&noise);
        int row = 0; // строки в файле, для оси X
        if(!noise.open(QFile::ReadOnly | QFile::Text))
        {
            qDebug() << "Data file not exists";
            QMessageBox::warning(this,"Waring","Data file  not exist");
        }
        else
        {
            QLineSeries *tempPar = new QLineSeries();
            QLineSeries *tempPer = new QLineSeries();
            QVector<int> vec1;
            QVector<int> vec2;
            while (!inNoise.atEnd())
            {
                QString dNoise = inNoise.readLine();
                QStringList lstNoise = dNoise.split(';');
                tempPar->append(row,lstNoise.at(0).toInt());
                tempPer->append(row,lstNoise.at(1).toInt());
                //sParWithOutNosie->append(row,dPar);
                //sPerWithOutNosie->append(row,dPer);

                row++;
            }

            tempPar->removePoints(0, this->minIndexPar - 1);
            tempPer->removePoints(0, this->minIndexPar - 1);

            // делаем так, чтобы серия из данных шума отсортировалась
            for(int i = 0; i < tempPar->count() && i < tempPer->count(); i ++)
            {
                this->sParWithOutNosie->append(i,this->sParWithOutNull->at(i).y() - tempPar->at(i).y());
                this->sPerWithOutNosie->append(i,this->sPerWithOutNull->at(i).y() - tempPer->at(i).y());

                if(this->maxPar < this->sParWithOutNosie->at(i).y())
                    this->maxPar = this->sParWithOutNosie->at(i).y();
                if(this->maxPer < this->sPerWithOutNosie->at(i).y())
                    this->maxPer = this->sPerWithOutNosie->at(i).y();

            }
            noise.close();
            // Создаётся график и добавляется в него серия значений
           addChart(sParWithOutNosie, sPerWithOutNosie,"Уадаление шумовой дорожки");

        }
}

void MainWindow::invert()
{
    for (int i = 0; i < this->sParWithOutNosie->count() && i < this->sPerWithOutNosie->count(); i++) {
        this->sParWithOutNosieInv->append(i, this->maxPar - this->sParWithOutNosie->at(i).y());
        this->sPerWithOutNosieInv->append(i, this->maxPer - this->sPerWithOutNosie->at(i).y());
    }
    addChart(this->sParWithOutNosieInv, this->sPerWithOutNosieInv,"Инвертированные значения");
}

void MainWindow::normValue()
{
    int signal_max =ui->signalMax->value();
    int tMaxPar = findMax(this->sParWithOutNosieInv);
    int tMaxPer = findMax(this->sPerWithOutNosieInv);
    for(int i = 0; i < this->sParWithOutNosieInv->count() && this->sParWithOutNosieInv->count(); i++)
    {
        int tmpPar = map(this->sParWithOutNosieInv->at(i).y(),0,tMaxPar, 0,signal_max);
        int tmpPer = map(this->sPerWithOutNosieInv->at(i).y(),0,tMaxPer, 0,signal_max);
        this->sParNorm->append(i,tmpPar);
        this->sPerNorm->append(i,tmpPer);
    }
    addChart(this->sParNorm, this->sPerNorm,"Нормирование по значениям");
}

void MainWindow::normKiloMetrs()
{
    int koefKillometrs = KILLOMETRS/this->sParNorm->count(); // берем "весовой" коэфф шага измерения.
    for (int i = 0; i < this->sParNorm->count() && this->sPerNorm->count(); i++)
    {
        int ParX = map(this->sParNorm->at(i).x() * koefKillometrs, 0, tMaxPer, 0,KILLOMETRS);
        int ParY = this->sParNorm->at(i).y();
        int PerX = this->sPerNorm->at(i).x() * koefKillometrs;
        int PerY = this->sPerNorm->at(i).y();
        this->sParNorm12->append(ParX, ParY);
        this->sPerNorm12->append(PerX, PerY);
    }

    addChart(this->sParNorm12, this->sPerNorm12, "Нормировка на 12 километров");
}

void MainWindow::addChart(QLineSeries *sPar, QLineSeries *sPer,QString title)
{
    QChartView *chartView = new QChartView(this);
    QChart *chart = new QChart();
    chart->addSeries(sPar);
    chart->addSeries(sPer);

    sPar->setName("\"Параллельный АЛ \" ");
    sPer->setName("\"Перпендикулярный АЛ\" ");

    chart->setTitle(title);
    // Настраиваются оси графика
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("X, м");            //Подпись к оси X
    axisX->setLabelFormat("%d");
    axisX->setTickCount(20);                 //Сколько отрезков на оси, минимум 2
    chart->addAxis(axisX, Qt::AlignBottom); //Подключение оси к графику
    sPar->attachAxis(axisX);              //Подключение оси к значениям
    sPer->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("t, мс");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(20);
    chart->addAxis(axisY, Qt::AlignLeft);
    sPar->attachAxis(axisY);
    sPer->attachAxis(axisY);
    chartView->setRubberBand(QChartView::HorizontalRubberBand);

    int Max = (findMax(sPar) > findMax(sPer) ? findMax(sPar) : findMax(sPer));
    int Min = (findMin(sPar) < findMin(sPer) ? findMin(sPar) : findMin(sPer));
    axisY->setMax(Max + 100);
    axisY->setMin(Min);

    chartView->setChart(chart);     // Устанавливаеncz график в представление
    ui->tabGraphics->addTab(chartView, title);
}

int MainWindow::findMin(QLineSeries *series)
{
    int min = series->at(0).y();
    for (int i = 0;i < series->count(); i++) {
        if(min > series->at(i).y())
            min = series->at(i).y();
    }
    qDebug() << min;
    return min;
}



int MainWindow::findMax(QLineSeries *series)
{
    int max = series->at(0).y();
    for (int i = 0;i < series->count(); i++) {
        if(max < series->at(i).y())
            max = series->at(i).y();
    }

    return max;
}

int MainWindow::findMaxX(QLineSeries *series)
{
    int max = series->at(0).x();
    for (int i = 0;i < series->count(); i++) {
        if(max < series->at(i).x())
            max = series->at(i).x();
    }
    return max;
}

int MainWindow::findMinIndex(QLineSeries *series)
{
    int min = series->at(0).y();
    int index = 0;
    for (int i = 0;i < series->count(); i++) {
        if(min > series->at(i).y())
        {
            min = series->at(i).y();
            index = i;
        }
    }
    qDebug() << index;
    return index;
}

int MainWindow::map(int value, int in_min, int in_max, int out_min, int out_max)
{
    return (value - in_min)*(out_max - out_min)/(in_max - in_min)+out_min;
}

