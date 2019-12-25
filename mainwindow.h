#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "winusbdriver.h"
#include "ui_mainwindow.h"
#include "imu.h"

#include "glview.h"
#include "setting.h"

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
    
    
    GLView *glview;
    IMU *imu;
    WinUSBDriver winusb;
    WinUSBDriver *qwindriver;
    QTimer *timer;
    Ui::MainWindow *ui;
    QLabel *imuStatusLabel;
    
private slots:
    void usbRecvSlot(unsigned char *buf,int len);
    void disconnectSlot(void);
    void on_pb_open_clicked();
    void on_pb_send_clicked();
    void onTimeOut();    
    void on_pb_cam_start_clicked();
    void on_pb_cam_stop_clicked();
    void on_pb_imu_start_clicked();
    void on_pb_imu_stop_clicked();
    void imuSlot(unsigned char *imu_data);
    void on_pb_imu_calibration_clicked();
};

#endif // MAINWINDOW_H
