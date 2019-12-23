#ifndef USBTHREAD_H
#define USBTHREAD_H

#include <QThread>
#include "libusb.h"
#include "workspace.h"

class WinUSBDriver;
#include "winusbdriver.h"

class USBThread : public QThread
{
    Q_OBJECT
private:
    
    WinUSBDriver *qwindriver;
    bool is_loop;
    
public:
    USBThread();
    void init(WinUSBDriver *qwindriver);
    void run();
    void waitClose();
    

};

#endif // USBTHREAD_H
