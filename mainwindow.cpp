#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sPerpen = new QLineSeries();
    sParallel = new QLineSeries();
    sParWithOutNosie = new QLineSeries();
    sPerWithOutNosie = new QLineSeries();
    sParWithOutNosieInv = new QLineSeries();
    sPerWithOutNosieInv = new QLineSeries();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnGo_clicked()
{
    begin();
    delNoise();
    invert();
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
            //vect1.append(lstData.at(0).toInt());
            //vect2.append(lstData.at(1).toInt());

            row++;
        }

        file.close();
        // Создаётся график и добавляется в него серия значений
        QChart *chart = new QChart(); // Для начлаьных значений

        chart->addSeries(sParallel);
        chart->addSeries(sPerpen);

        sParallel->setName("\"Параллельный АЛ \" ");
        sPerpen->setName("\"Перпендикулярный АЛ\" ");

        chart->setTitle("Начальные значения");
        // Настраиваются оси графика
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("X, м");            //Подпись к оси X
        axisX->setLabelFormat("%d");
        axisX->setTickCount(20);                 //Сколько отрезков на оси, минимум 2
        chart->addAxis(axisX, Qt::AlignBottom); //Подключение оси к графику
        sParallel->attachAxis(axisX);              //Подключение оси к значениям
        sPerpen->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("t, мс");
        axisY->setLabelFormat("%d");
        axisY->setTickCount(20);
        chart->addAxis(axisY, Qt::AlignLeft);
        sParallel->attachAxis(axisY);
        sPerpen->attachAxis(axisY);

        axisY->setMax(this->sParallel->at(sParallel->count()-1).y() + 100);
        //axisY->setMin(sParallel);

        chartView->setChart(chart);     // Устанавливаеncz график в представление
        ui->tabGraphics->addTab(chartView, "Начальные значения");
        //ui->tabGraphics->addTab(chartViewInv, "Инвертированное значения");
        //-----------------------------------------------------
    }
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
            // делаем так, чтобы серия из данных шума отсортировалась
            for(int i = 0; i < tempPar->count(); i ++)
            {
                this->sParWithOutNosie->append(i,this->sParallel->at(i).y() - tempPar->at(i).y());
                this->sPerWithOutNosie->append(i,this->sPerpen->at(i).y() - tempPer->at(i).y());
                if(this->maxPar < abs(this->sParWithOutNosie->at(i).y()))
                    this->maxPar = abs(this->sParWithOutNosie->at(i).y());
                if(this->maxPer < abs(this->sPerWithOutNosie->at(i).y()))
                    this->maxPer = abs(this->sPerWithOutNosie->at(i).y());
            }
            qDebug() << this->maxPar;
            qDebug() << this->maxPer;

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
void MainWindow::addChart(QLineSeries *sPar, QLineSeries *sPer,QString title)
{
    QChartView *chartView = new QChartView(this);
    QChart *chart = new QChart();
    chart->addSeries(sPar);
    chart->addSeries(sPer);

    sPar->setName("\"Параллельный АЛ \" ");
    sPer->setName("\"Перпендикулярный АЛ\" ");

    chart->setTitle("Начальные значения");
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
    //axisY->setMax(this->sParalle->at(sParallel->count()-1).y() + 100);
   // axisY->setMin(0);

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
