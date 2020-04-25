#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>
#include "C:/study/qt/QSocket/Lab2_client/myclient.h"
#include <QMessageBox>
#include <form.h>
class MyClient;
//class Form;
namespace Ui {
    class Dialog;

}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    //определим слоты для обработки сигналов сокета
    void onSokConnected();
    void onSokDisconnected();
    //сигнал readyRead вызывается, когда сокет получает пакет (который может быть лишь частью отправленых данных) байтов
    void onSokReadyRead();
    void onSokDisplayError(QAbstractSocket::SocketError socketError);
    //cигнал передачи с сокета данных
   // void to_file(QByteArray data);
    void on_pbConnect_clicked();
    void on_pbDisconnect_clicked();

    void on_pbSend_clicked();

   // void on_leName_cursorPositionChanged(int arg1, int arg2);
    void WithFile(QByteArray data);

    void on_OpenFile_clicked();
    void graphic_signals();

    void on_comboBox_activated(const QString &arg1);

private:
    Ui::Dialog *ui;
    QTcpSocket *_sok; //сокет
    quint16 _blockSize; //текущий размер блока данных
    Form *_forma;
    void AddToLog(QString text, QColor color = Qt::black);




};

#endif // DIALOG_H
