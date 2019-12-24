#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->setWindowTitle("OPENVIO上位机");
    
    connect(&winusb,SIGNAL(recvSignals(unsigned char *,int)),this,SLOT(usbRecvSlot(unsigned char *,int)));
    connect(&winusb,SIGNAL(disconnectSignals()),this,SLOT(disconnectSlot()));
    connect(&winusb,SIGNAL(imuSignals(unsigned char *)),this,SLOT(imuSlot(unsigned char *)));
    
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timer->start(1000);
    
    //imuStatusLabel = new QLabel("imuStatus");
    //ui->statusBar->addPermanentWidget(imuStatusLabel);
    
    GLView *glview = new GLView(this);
    
    ui->verticalLayout->addWidget(glview,0,0);
    
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::usbRecvSlot(unsigned char *buf,int len)
{
    QImage myImage = QImage(winusb.img.img,winusb.img.width,winusb.img.high,QImage::Format_Grayscale8);
    ui->lb_img->setPixmap(QPixmap::fromImage(myImage));
}

void MainWindow::on_pb_open_clicked()
{
    if(ui->pb_open->text().compare(QString::fromUtf8("打开")) == 0)
    {
        int vid = ui->le_vid->text().toInt();
        int pid = ui->le_pid->text().toInt();
        if(winusb.open(vid,pid) == 0)
        {
            ui->pb_open->setText("关闭");
        }
    }else
    {
        winusb.close();
        ui->pb_open->setText("打开");
    }   
}
void MainWindow::disconnectSlot(void)
{
    winusb.close();
    ui->pb_open->setText("打开");
}

void MainWindow::imuSlot(unsigned char *imu_data)
{
    static short acc[3],gyro[3],temp;
    
    acc[0] = (short)((imu_data[0]<<8)|imu_data[1]);
    acc[1] = (short)((imu_data[2]<<8)|imu_data[3]);
    acc[2] = (short)((imu_data[4]<<8)|imu_data[5]);
    
    temp = (short)((imu_data[6]<<8)|imu_data[7]);
    
    gyro[0] = (short)((imu_data[8]<<8)|imu_data[9]);
    gyro[1] = (short)((imu_data[10]<<8)|imu_data[11]);
    gyro[2] = (short)((imu_data[12]<<8)|imu_data[13]);
    
    ui->lb_acc_x->setText(QString::number(acc[0]));
    ui->lb_acc_y->setText(QString::number(acc[1]));
    ui->lb_acc_z->setText(QString::number(acc[2]));
    
    ui->lb_gyr_x->setText(QString::number(gyro[0]));
    ui->lb_gyr_y->setText(QString::number(gyro[1]));
    ui->lb_gyr_z->setText(QString::number(gyro[2]));
    
    ui->lb_temp->setText(QString::number(temp));
    
}

void MainWindow::on_pb_send_clicked()
{
    //QByteArray bytes = ui->le_send->text().toUtf8();
    //winusb.send(bytes);
    
    //winusb.ctrl();
}

void MainWindow::onTimeOut()
{
    QString str;
    if(winusb.recv_count_1s < 1024)
    {
        str = QString::number(winusb.recv_count_1s);
        str += "B/s";
    }else if(winusb.recv_count_1s < 1024*1024)
    {
        str = QString::number(winusb.recv_count_1s/1024);
        str += "KB/s";
    }else if(winusb.recv_count_1s < 1024*1024*1024)
    {
        str = QString::number(winusb.recv_count_1s/1024/1024);
        str += "MB/s";
    }
    
    str += " ";
    str += QString::number(winusb.frame_fps);
    str += "fps";
    winusb.frame_fps = 0;
    
    winusb.recv_count_1s = 0;
    ui->statusBar->showMessage(str);
    
    ui->lb_imu_hz->setText(QString::number(winusb.imu_hz)+"Hz");
    winusb.imu_hz = 0;
}

void MainWindow::on_pb_cam_start_clicked()
{
    winusb.ctrlCamStart();
}

void MainWindow::on_pb_cam_stop_clicked()
{
    winusb.ctrlCamStop();
}

void MainWindow::on_pb_imu_start_clicked()
{
    winusb.ctrlIMUStart();
}

void MainWindow::on_pb_imu_stop_clicked()
{
    winusb.ctrlIMUStop();
}
