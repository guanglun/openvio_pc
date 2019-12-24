#include "usbthread.h"

USBThread::USBThread()
{

    
    is_loop = true;
}

void USBThread::init(WinUSBDriver *qwindriver,int which)
{
    this->qwindriver = qwindriver;
    this->which = which;
}

void USBThread::run()
{
    is_loop = true;
    DBG("usb thread start");
    //while(is_loop)
    {
        if(which == 0)
        {
            qwindriver->CamRecv();
        }else{
            qwindriver->IMURecv();
        }
        //DBG("thread");
        //libusb_handle_events(NULL);
        //msleep(1);
    }
    DBG("usb thread exit");
    is_loop = false;
}

void USBThread::waitClose()
{
    while(is_loop);
}


