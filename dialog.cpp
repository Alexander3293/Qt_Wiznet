#include "dialog.h"
#include "ui_dialog.h"

#include <QtGui>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <string.h>

#define OUT_TAKT_ETHERNET       0xa1 //external timing ETHERNET
#define OUT_TAKT_UART           0xa3 //external timing UART
#define HSE_TIMER_UART          0xa5
#define HSE_TIMER_ETHERNET      0xa7 //timing HSE

#define SINGLE_ADC2				0xa9 // Single ADC2

#define FREQUENCE_1_kHz_adc1	0xb1 //1 kilo
#define FREQUENCE_10_kHz_adc1	0xb4
#define FREQUENCE_50_kHz_adc1	0xb7

#define DOUBLE_ADC_FREQ_1kHz	0xd0 //Совместная работа двух АЦП 1 и 2
#define DOUBLE_ADC_FREQ_10kHz	0xd4
#define DOUBLE_ADC_FREQ_50kHz	0xd8

#define OUT_DOUBLE_ADC          0xda
#define OUT_TAKT_adc1           0xdc
#define UART1_ENA				0xe7 //Включим работу с UART1



QString choosing_adc = "single"; //В файле будет прописан вид АЦП чтобы изначально понимать, что в файле




Dialog::Dialog(QWidget *parent) :QDialog(parent),ui(new Ui::Dialog)
{
    ui->setupUi(this);



    _sok = new QTcpSocket(this);
    _forma = new Form();
    connect(_sok, SIGNAL(readyRead()), this, SLOT(onSokReadyRead()));
    connect(_sok, SIGNAL(connected()), this, SLOT(onSokConnected()));
    connect(_sok, SIGNAL(disconnected()), this, SLOT(onSokDisconnected()));
    connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSokDisplayError(QAbstractSocket::SocketError)));
    connect(_forma, &Form::graphic_show, this, &Dialog::show);


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onSokDisplayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(this, "Error", "The following error occurred: "+_sok->errorString());
    }
}

void Dialog::onSokReadyRead()
{
    QDataStream in(_sok);
    quint64 Byt =  _sok->bytesAvailable() ;
    //qDebug() << "Skolko  Byte dostupno " << Byt;


    QByteArray message = _sok->readAll();
    ui->pbSend->setEnabled(true);
  //  qDebug() << "READING " << message.toHex();

    if(ui->comboBox_2->currentText() == "UART ENABLE")     //Считываем все, что пришлос  UART
    {
         AddToLog("[ "+ QString::number(Byt) + " ]:" + message.toHex(), Qt::red);
         return;
    }

   //Если длина сообщения меньше 50, то отправляем одиночное измерение ADC
    if(message.length() <= 50)
    {
        bool ok;
        QByteArray message_short = message.left(4); //Берем, первые 4 символа
        int iVal = message_short.toInt(&ok,16);     //Переводим из Hex в int
        QString str = QString::number(iVal);        // to Str
        ui->lineEdit->clear();                      //Отчищаем старые измерения
        ui->lineEdit->setText(str);
        AddToLog("[ "+ QString::number(Byt) + " ]:" + message.toHex(), Qt::red);
        return;
    }

    AddToLog("[ "+ QString::number(Byt) + " ]:" + message.toHex(), Qt::red);

    WithFile(message);


}

void Dialog::onSokConnected()
{


    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(true);
    ui->pbSend->setEnabled(true);
    _blockSize = 0;
    AddToLog("Connected to"+_sok->peerAddress().toString()+":"+QString::number(_sok->peerPort()),Qt::green);

    //try autch
   // QByteArray block;
   // QDataStream out(&block, QIODevice::WriteOnly);
//    out << (quint16)0;
 //   out << (quint8)MyClient::comAutchReq;
//    out << ui->leName->text();

    //out.device()->seek(0);
    //out << (quint16)(block.size() - sizeof(quint16));
    //_sok->write(block);
}

void Dialog::onSokDisconnected()
{
    ui->pbConnect->setEnabled(true);
    ui->pbDisconnect->setEnabled(false);
    ui->pbSend->setEnabled(false);

    AddToLog("Disconnected from"+_sok->peerAddress().toString()+":"+QString::number(_sok->peerPort()), Qt::green);
}

void Dialog::on_pbConnect_clicked()
{
    _sok->connectToHost(ui->leHost->text(), ui->sbPort->value());
}

void Dialog::on_pbDisconnect_clicked()
{
    _sok->disconnectFromHost();
}



void Dialog::on_pbSend_clicked()
{
    QString text_in_comboBox = "frequence";
    if(ui->comboBox->currentText() == text_in_comboBox) //if press "frequence" output Error
    {
        QMessageBox::information(this,"title", "CHOOSE FREQUENCE");
        return;
    }

    if(ui->comboBox_2->currentText() == "Choose Periphery")
    {
        QMessageBox::information(this,"title", "Choose Periphery");
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    //out << (quint16)7;
    // out << (quint8)MyClient::comMessageToAll;
    QString combo_text = ui->comboBox->currentText();//��������� ��� ������
    QString combo_text_perif = ui->comboBox_2->currentText();

    /* 1 kHz for Timer ADC1 */
    if((combo_text=="ADC1 1kHz"))
    {
        choosing_adc = "single";
        if(combo_text_perif == "Ethernet")
        {
            out <<(quint8)HSE_TIMER_ETHERNET;
            out <<(quint8)FREQUENCE_1_kHz_adc1;
        }
        else if(combo_text_perif == "UART")
        {
            out <<(quint8)HSE_TIMER_UART;
            out <<(quint8)FREQUENCE_1_kHz_adc1;
        }

    }

    /* 10 kHz for Timer ADC1 */

    else if(combo_text=="ADC1 10kHz")
    {
        choosing_adc = "single";
        if(combo_text_perif == "Ethernet")
        {
        out <<(quint8)HSE_TIMER_ETHERNET;
        out <<(quint8)FREQUENCE_10_kHz_adc1;
        }
        else if(combo_text_perif == "UART")
        {
            out<<(quint8)HSE_TIMER_UART;
            out<<(quint8)FREQUENCE_10_kHz_adc1;
        }
    }

    /* 50 kHz for Timer ADC1*/

    else if(combo_text=="ADC1 50kHz")
    {
        choosing_adc = "single";
        if(combo_text_perif == "Ethernet")
        {
        out <<(quint8)HSE_TIMER_ETHERNET;
        out <<(quint8)FREQUENCE_50_kHz_adc1;
        }
        else if(combo_text_perif == "UART")
        {
            out<<(quint8)HSE_TIMER_UART;
            out<<(quint8)FREQUENCE_50_kHz_adc1;
        }
    }

    /*External begin *for ADC1*/

    else if(combo_text=="EXT ADC1")
    {
        choosing_adc = "single";
        if(combo_text_perif == "Ethernet")
        {
            out <<(quint8)HSE_TIMER_UART;
            out <<(quint8)OUT_TAKT_adc1;
        }
        else if(combo_text_perif == "UART")
        {
            out<<(quint8)HSE_TIMER_UART;
            out<<(quint8)(quint8)OUT_TAKT_adc1;

        }
    }

    else if(combo_text=="SINGLE ADC2")
    {
        choosing_adc = "single";
        if(combo_text_perif == "Ethernet")
        {
        out <<(quint8)HSE_TIMER_ETHERNET;
        out <<(quint8)SINGLE_ADC2;
        }
        else if(combo_text_perif == "UART")
        {
            out <<(quint8)HSE_TIMER_UART;
            out <<(quint8)SINGLE_ADC2;
        }

    }
    else if(combo_text=="Double ADC 1kHz")
    {
        choosing_adc = "double";
        if(combo_text_perif == "Ethernet")
        {
            out <<(quint8)HSE_TIMER_ETHERNET;
            out <<(quint8)DOUBLE_ADC_FREQ_1kHz;
        }
        else if(combo_text_perif == "UART")
        {
            out <<(quint8)HSE_TIMER_UART;
            out <<(quint8)DOUBLE_ADC_FREQ_1kHz;
        }
    }
    else if(combo_text=="Double ADC 10kHz")
    {
        choosing_adc = "double";
        if(combo_text_perif == "Ethernet")
        {
            out <<(quint8)HSE_TIMER_ETHERNET;
            out <<(quint8)DOUBLE_ADC_FREQ_10kHz;
        }
        else if(combo_text_perif == "UART")
        {
            out <<(quint8)HSE_TIMER_UART;
            out <<(quint8)DOUBLE_ADC_FREQ_10kHz;
        }
    }
    else if(combo_text=="Double ADC 50kHz")
    {
        choosing_adc = "double";
        if(combo_text_perif == "Ethernet")
        {
            out <<(quint8)HSE_TIMER_ETHERNET;
            out <<(quint8)DOUBLE_ADC_FREQ_50kHz;
        }
        else if(combo_text_perif == "UART")
        {
           out<<(quint8)HSE_TIMER_UART;
           out <<(quint8)DOUBLE_ADC_FREQ_50kHz;
        }
    }
    else if(combo_text=="OUT_DOUBLE_ADC")
    {
        choosing_adc = "double";
        if(combo_text_perif == "Ethernet")
        {
             out<<(quint8)HSE_TIMER_UART;
             out <<(quint8)OUT_DOUBLE_ADC;
        }
        else if(combo_text_perif == "UART")
        {
            out<<(quint8)HSE_TIMER_UART;
            out<<(quint8)OUT_DOUBLE_ADC;

        }

    }

    else if(combo_text=="UART ENABLE")
    {
         out <<(quint8)UART1_ENA;
         QString str = ui->pteMessage->document()->toPlainText(); //Получим текст из pteMessage
         if(str != "")
         {
             out << str;
             str = "";
         }



    }
//    out << ui->pteMessage->document()->toPlainText();
   // out.device()->seek(0);
    //out << (quint16)(block.size() - sizeof(quint16));



    _sok->write(block);
    ui->pteMessage->clear();
}

void Dialog::AddToLog(QString text, QColor color)
{
    ui->lwLog->insertItem(0, QTime::currentTime().toString()+" "+text);
    ui->lwLog->item(0)->setTextColor(color);
}

void Dialog::WithFile(QByteArray data)
{



    QString logFileName = QString("D:/Test/Log_%1__%2__%3.txt")
                     .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                     .arg(QTime::currentTime().toString("hh_mm_ss_zzz"))
                     .arg(choosing_adc); //file with data

       QFile file(logFileName);
    //if(!file.exists())   qDebug() << "NO FILE " ;

    if(!file.open(QIODevice::Append |QIODevice::WriteOnly)) qDebug() << "NOT OPEN " ;
    QTextStream OutputStream(&file);
    OutputStream << data.toHex();
    file.close();

}


void Dialog::on_OpenFile_clicked()
{

    QFile("D:/Test/mV.txt").remove();
    QString file_name_mV = QString("D:/Test/mV.txt");
    QFile file_mV(file_name_mV); // в Этот файл записываю значения АЦП


    if(!file_mV.open(QIODevice::Append |QIODevice::WriteOnly)) qDebug() << "NOT OPEN " ;
    QTextStream OutputStream(&file_mV);

    uint time_clock=0;
    uint counter_time=0;


    QString filter = "Txt File(*.txt)";
    QString file_name = QFileDialog::getOpenFileName(this, "Open a File", QDir::homePath(), filter);

    /*В начало файла mv запишем цифры, при котором идентифицируем вид АЦП*/
    QString str = "single";
    if(file_name.indexOf(str) > -1) //return -1, если не найдено ничего
    {
        OutputStream << 60000;  //single
        OutputStream << '\n';
    }
    else
    {
        OutputStream << 10000;  //multimode
        OutputStream << '\n';
    }

    QFile file(file_name);


    if(!file.open(QFile::ReadOnly|QFile::Text)) //if file, not open
    {
        QMessageBox::information(this,"title", "file not open");
    }
    //const double cof = 3300.0/4095.0;
   // double value_mV =0;
            while(!file.atEnd()) // leave, if End of file
            {
                bool ok;

            QByteArray data = file.read(4);
            QString str = data;
/*
 *
            packet_2[0] = 0x00;
            packet_2[1] = 0x00;
            packet_2[2] = 0xea;
            packet_2[3] = 0x60;

            packet_2[4] = time_clock >> 24;
            packet_2[5] = time_clock >> 16;
            packet_2[6] = time_clock >> 8;
            packet_2[7] = time_clock;

            packet_2[8]= counter_time >> 8;
            packet_2[9]= counter_time;
 *
 */
           int iVal = str.toInt(&ok,16);
           if(iVal==0)
           {
                str = file.read(4);
                iVal = str.toInt(&ok,16);
               if(iVal == 60000)  //Start of the package
              {
                   str = file.read(8);  // clock time
                   time_clock = str.toInt(&ok,16);

                   str = file.read(4); // counter time
                   counter_time = str.toInt(&ok,16);
                   if(time_clock)
                   {
                       qDebug()<<"Time:" << time_clock;
                       qDebug()<<"Counter:" << counter_time;
                   }

               }

               else
               {
                   OutputStream << iVal;
                   OutputStream << '\n';
               }
           }
           else
           {
               OutputStream << iVal;
               OutputStream << '\n';
           }




            }
    file_mV.close();
    file.close();

    //graphic_signals();
   _forma->show(); //for paint graphic
}

void Dialog::graphic_signals()
{

}

void Dialog::on_comboBox_activated(const QString &arg1)
{
    QString text_in_comboBox = "frequence";
    ui->comboBox->currentText();
    if(arg1 == text_in_comboBox) //
    {
        QMessageBox::information(this,"title", "CHOOSE FREQUENCE");
        return;
    }
}


