#include "form.h"
#include "ui_form.h"
#include <QFile>
#include <QFileDialog>
#include <QtGui>
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)

{

    ui->setupUi(this);
  //  this->setWindowFlags(Qt::WindowStaysOnTopHint);


}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
     QString file_name = QString("D:/Test/mV.txt");
     QFile file(file_name);

     if(!file.open(QFile::ReadOnly|QFile::Text)) //Открываем для чтения
     {
         QMessageBox::information(this,"title", "file not open"); //Если нельзя открыть, выводим данное сообщение

     }
     QString data;
     data = file.readLine();
     file.close();
     if(data=="60000\n")
     {
          single_graphics();
     }
     else if(data == "10000\n")
     {
          multimode_graphics();
     }



}

void Form::multimode_graphics()
{
    QString file_name = QString("D:/Test/mV.txt");
     QFile file(file_name);


     if(!file.open(QFile::ReadOnly|QFile::Text)) //Открываем для чтения
     {
         QMessageBox::information(this,"title", "file not open"); //Если нельзя открыть, выводим данное сообщение

     }
 QByteArray data;
  //Вычисляем количество точек, которые будем отрисовывать
int N = ui->lineEdit->text().toUInt();
int begin = ui->lineEdit_2->text().toUInt();
     QVector<double> x(N), y1(N), y2(N); //Массивы координат точек
 int i=0; int k=0;
 int l=0; //счетчик числа отсчетов АЦП
 data = file.readLine();
             while(!file.atEnd()) //Считываем файл, пока не считаем весь файл
             {
                while(k!=begin && !file.atEnd())
                {
                    data = file.readLine();     //Range file от K до N
                    k++;
                }

//                data = file.readLine(begin);
//                y1[i] = data.toInt();   //ADC *
//                y2[i+1] = data.toInt(); //ADC *
//                x[l] = l+begin;
//                i+=2;                   //Т.к считываем сразу 2 значения АЦП, то +=2
//                l++;
//                if(l==N) break;


                data = file.readLine(begin);

                if(i%2 == 0) y1[l] = data.toInt();

                else if(i%2 == 1)
                {
                    y2[l] = data.toInt();
                    l++;
                    x[l] = l+begin;
                }
                i++;
                if(l==(N-1)) break;
             }

     file.close();

         ui->widget->clearGraphs();//Если нужно, но очищаем все графики
         //Добавляем один график в widget
         ui->widget->addGraph();
         //Говорим, что отрисовать нужно график по нашим двум массивам x и y
         ui->widget->graph(0)->setPen(QPen(Qt::red));
         ui->widget->graph(0)->setData(x, y1);

         ui->widget->addGraph();
         ui->widget->graph(1)->setPen(QPen(Qt::green));
         ui->widget->graph(1)->setData(x, y2);

         //Подписываем оси Ox и Oy
         ui->widget->xAxis->setLabel("Otchet");
         ui->widget->yAxis->setLabel("Range");

         //Установим область, которая будет показываться на графике
         ui->widget->xAxis->setRange(begin, N+begin);//Для оси Ox

         //Для показа границ по оси Oy сложнее, так как надо по правильному
         //вычислить минимальное и максимальное значение в векторах
         double minY1 = y1[0], maxY1 = y1[0];
         double minY2 = y2[0], maxY2 = y2[0];
         for (int i=1; i<N; i++)
         {
             if (y1[i]<minY1) minY1 = y1[i];
             if (y1[i]>maxY1) maxY1 = y1[i];

             if (y2[i]<minY2) minY2 = y2[i];
             if (y2[i]>maxY2) maxY2 = y2[i];
         }
         if(minY1>minY2) minY1 = minY2;
         if(maxY1<maxY2) maxY1 = maxY2;
         ui->widget->yAxis->setRange(minY1, maxY1);//Для оси Oy

         //И перерисуем график на нашем widget
         ui->widget->replot();
}

void Form::single_graphics()
{
    QString file_name = QString("D:/Test/mV.txt");
     QFile file(file_name);


     if(!file.open(QFile::ReadOnly|QFile::Text)) //Открываем для чтения
     {
         QMessageBox::information(this,"title", "file not open"); //Если нельзя открыть, выводим данное сообщение

     }
 QByteArray data;
  //Вычисляем количество точек, которые будем отрисовывать
int N = ui->lineEdit->text().toUInt();
int begin = ui->lineEdit_2->text().toUInt();
     QVector<double> x(N), y(N); //Массивы координат точек
 int i=0; int k=0;
 data = file.readLine();
             while(!file.atEnd()) //Считываем файл, пока не считаем весь файл
             {
                while(k!=begin && !file.atEnd())
                {
                    data = file.readLine();
                    k++;
                }

                data = file.readLine(begin);
                y[i] = data.toInt();
                x[i] = i+begin;
                i++;
                if(i==N) break;


             }

     file.close();

         ui->widget->clearGraphs();//Если нужно, но очищаем все графики
         //Добавляем один график в widget
         ui->widget->addGraph();
         //Говорим, что отрисовать нужно график по нашим двум массивам x и y
         ui->widget->graph(0)->setData(x, y);

         //Подписываем оси Ox и Oy
         ui->widget->xAxis->setLabel("Otchet");
         ui->widget->yAxis->setLabel("Range");

         //Установим область, которая будет показываться на графике
         ui->widget->xAxis->setRange(begin, N+begin);//Для оси Ox

         //Для показа границ по оси Oy сложнее, так как надо по правильному
         //вычислить минимальное и максимальное значение в векторах
         double minY = y[0], maxY = y[0];
         for (int i=1; i<N; i++)
         {
             if (y[i]<minY) minY = y[i];
             if (y[i]>maxY) maxY = y[i];
         }
         ui->widget->yAxis->setRange(minY, maxY);//Для оси Oy

         //И перерисуем график на нашем widget
         ui->widget->replot();
}
