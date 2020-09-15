#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnGo_clicked()
{
    QFile file("2017-04-11_-_14-06-06_4_raw.csv");
    if ( !file.open(QFile::ReadOnly | QFile::Text) )
    {
        qDebug() << "File not exists";
        QMessageBox::warning(this,"Waring","File not exist");
    }

    else {
        // Создаём поток для извлечения данных из файла
        QTextStream in(&file);
        QtCharts::QChartView *chartView = new QtCharts::QChartView(this);
        QtCharts::QLineSeries *sParallel = new QtCharts::QLineSeries();
        QtCharts::QLineSeries *sPerpen = new QtCharts::QLineSeries();

        // Считываем данные до конца файла
        int row = 0; // строки в файле, для оси X
        while (!in.atEnd())
        {
            // ... построчно
            QString line = in.readLine();
            // Добавляем в модель по строке с элементами
            //QList<QStandardItem *> standardItemsList;
            // учитываем, что строка разделяется точкой с запятой на колонки
            //for (QString item : line.split(";")) {
                //standardItemsList.append(new QStandardItem(item));
            //}
            QStringList lst = line.split(';');
            sParallel->append(row,lst.at(0).toInt());
            sPerpen->append(row,lst.at(1).toInt());
            row++;
            //qDebug() << series->points();
            //csvModel->insertRow(csvModel->rowCount(), standardItemsList);
        }
        file.close();
        // Создаётся график и добавляется в него серия значений
        QChart *chart = new QChart();
        chart->addSeries(sParallel);
        chart->addSeries(sPerpen);
        sParallel->setName("\"Параллельный АЛ\" ");
        sPerpen->setName("\"Перпендикулярный АЛ\" ");
        //chart.children()
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
        //qDebug() << sizeof(sParallel-((sParallel + sParallel->count())));
        chartView->setChart(chart);     // Устанавливаеncz график в представление
        ui->tabGraphics->addTab(chartView, "Начальные значения");
        //-----------------------------------------------------
    }
}
