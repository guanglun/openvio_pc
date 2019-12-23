#include "usbthread.h"

USBThread::USBThread()
{

    
    is_loop = true;
}

void USBThread::init(WinUSBDriver *qwindriver)
{
    this->qwindriver = qwindriver;
}

void USBThread::run()
{
    is_loop = true;
    DBG("usb thread start");
    //while(is_loop)
    {
        qwindriver->recv();
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


