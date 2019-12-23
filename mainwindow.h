#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "winusbdriver.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    WinUSBDriver winusb;
    WinUSBDriver *qwindriver;
    QTimer *timer;
    Ui::MainWindow *ui;
    
private slots:
    void usbRecvSlot(unsigned char *buf,int len);
    void disconnectSlot(void);
    void on_pb_open_clicked();
    void on_pb_send_clicked();
    void onTimeOut();    
};

#endif // MAINWINDOW_H
