#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QtCharts/QAbstractSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QVector>
#include <cmath>
using namespace QtCharts;

#define KILLOMETRS 12000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QVector<int> vect1,vect2;
    int maxPar = 0; //макс занчение параллельного, используется для инвертирования
    int maxPer = 0; //макс занчение перпендикулятрного, используется для инвертирования
    int minIndexPar; // запоминаеи индексы минимальных элементов,
    int minIndexPer; // чтобы осключить такое же количество точек из шумовой дорожки
    bool end = false;

    QLineSeries *sPerpen;
    QLineSeries *sParallel;
    QLineSeries *sParWithOutNosie; // серии для отобажения графиков сигнала без шумовых дорожек
    QLineSeries *sPerWithOutNosie; //
    QLineSeries *sParWithOutNull;   // серии для отображения графиков сигнала без нулевой дорожки
    QLineSeries *sPerWithOutNull;
    QLineSeries *sParWithOutNosieInv; // серии для отображения графиков сигнала инвертирвоанных
    QLineSeries *sPerWithOutNosieInv;

    QLineSeries *sParNorm; // серии для отображения графиков нормированное по значениям сигнала
    QLineSeries *sPerNorm; //
    QLineSeries *sParNorm12; // серии для отображения графиков нормированных по расстоянию
    QLineSeries *sPerNorm12;

    QLineSeries *sMedianPar;// серии для результатов медианной фильтрации
    QLineSeries *sMedianPer;

    QLineSeries *sResult;

private slots:
    void on_btnGo_clicked();

private:
    Ui::MainWindow *ui;
    void invert(); // инвертирования сигнала
    void begin(); // начальное отображение сигнала
    void delNoise(); // удаление шумовой дорожки
    void dellZero(); // удаление
    void normValue(); // нормировка значений по промежутку от 0, до signal_max
    void normKiloMetrs(); // нормировка величин по 12 километрам
    void median(); // функция медианного фильтра 3 порядка
    void result();

    void addChart(QLineSeries *, QLineSeries*, QString title); // добавление вкладки с графиком
    void addChart(QLineSeries *, QString title); // добавление вкладки с графиком
    int map(int value, int in_min,int in_max,int out_min,int out_max); // перевод значения в заданный интервад
    int findMin(QLineSeries *); // поиск минимального значения в серии
    int findMax(QLineSeries *); // поиск максимального значения в серии
    int findMaxX(QLineSeries *);
    int findMinIndex(QLineSeries *); // return min vaue index
    QLineSeries *medianFilter(QLineSeries *initial); // функция медианного фильтра

};
#endif // MAINWINDOW_H
