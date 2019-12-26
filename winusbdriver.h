#ifndef WINUSBDRIVER_H
#define WINUSBDRIVER_H

#include <QObject>

#include "libusb.h"
#include "workspace.h"
#include "image.h"

class USBThread;
#include "usbthread.h"

#define CANDLE_MAX_DEVICES 32


#define RECV_LEN    (640*480)

enum SENSOR_STATUS{
    SENSOR_STATUS_STOP,
    SENSOR_STATUS_WAIT_HEAD,
    SENSOR_STATUS_RUNNING
};

class WinUSBDriver : public QObject
{
    Q_OBJECT
private:
    int recv_len_count = 0;
    
    int ret;
    
    unsigned char *ctrl_buffer,*imu_buffer;
    unsigned char recv_buf_tmp[RECV_LEN+1];
    int camRecvLen,imuRecvLen,recv_index;
    bool is_open;
    
    libusb_device **devs,*dev;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_device_descriptor desc;
    libusb_config_descriptor *cfg = NULL;
    struct libusb_transfer* m_xfer;
    
    USBThread *camThread,*imuThread;
    libusb_context* m_libusb_context;

    enum SENSOR_STATUS camStatus;
    
public:
    Image img;
    unsigned int recv_count_1s = 0,frame_fps = 0,imu_hz = 0;
    
    WinUSBDriver();
    ~WinUSBDriver();
    
    int open(int vid,int pid);
    int close(void);
    static void LIBUSB_CALL completeCallback(libusb_transfer *xfer);
    void send(QByteArray byte);
    void CamRecv(void);
    void IMURecv(void);
    int sendCtrl(char request,unsigned char *buffer,int len);
    void ctrlCamStart();
    void ctrlCamStop();
    void ctrlIMUStart();
    void ctrlIMUStop();
    
signals:
    void recvSignals(unsigned char *buf,int len);
    void disconnectSignals(void);
    void imuSignals(unsigned char *imu_data);
    void closeSignals(void);
    
private slots:
    void closeSlot(void);
};

#endif // WINUSBDRIVER_H
