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


    QLineSeries *sPerpen;
    QLineSeries *sParallel;
    QLineSeries *sParWithOutNosie;
    QLineSeries *sPerWithOutNosie;
    QLineSeries *sParWithOutNosieInv;
    QLineSeries *sPerWithOutNosieInv;

private slots:
    void on_btnGo_clicked();

private:
    Ui::MainWindow *ui;
    void invert(); // инвертирования сигнала
    void begin(); // начальное отображение сигнала

    void delNoise(); // удаление шумовой дорожки
    void dellZero(); // удаление
    void addChart(QLineSeries *, QLineSeries*, QString title); // добавление вкладки с графиком

    int map(int value, int in_min,int in_max,int out_min,int out_max); // перевод значения в заданный интервад
    int findMin(QLineSeries *); // поиск минимального значения в серии
    int findMax(QLineSeries *); // поиск максимального значения в серии
    int findMinIndex(QLineSeries *); // return min vaue index
};
#endif // MAINWINDOW_H
