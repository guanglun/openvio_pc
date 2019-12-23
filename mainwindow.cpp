#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->setWindowTitle("OPENVIO上位机");
    
    connect(&winusb,SIGNAL(recvSignals(unsigned char *,int)),this,SLOT(usbRecvSlot(unsigned char *,int)));
    connect(&winusb,SIGNAL(disconnectSignals()),this,SLOT(disconnectSlot()));
    
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timer->start(1000);
    
//    QImage myImage2 = QImage(240,480,QImage::Fo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::usbRecvSlot(unsigned char *buf,int len)
{
    
//    buf[len] = '\0';
    
//    //DBG("recv:%s",buf);
    
//    QString qstr = QString(QLatin1String((const char*)buf));
//    ui->te_recv->append(qstr);
//    ui->te_recv->moveCursor(QTextCursor::End);
    
    
    //winusb.img.CONVERT_YUV420PtoRGB24(winusb.img.img,winusb.img.img_rgb,winusb.img.width,winusb.img.high);
    
    //winusb.img.simplest_yuv420_graybar(winusb.img.width,winusb.img.high,0,255,10,(char *)winusb.img.img_rgb);
    
//    for(int i = 0;i<winusb.img.size;i++)
//    {
//        winusb.recv_buf[i] = 0xFF;
//    }

//    for(int i = 0;i<winusb.img.width*100;i++)
//    {
//        winusb.recv_buf[i] = 0x00;
//    }
    
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

void MainWindow::on_pb_send_clicked()
{
    QByteArray bytes = ui->le_send->text().toUtf8();
    winusb.send(bytes);
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
}
