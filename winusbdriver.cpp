#include "winusbdriver.h"
#define CTRL_EPADDR         0x01    
#define CAM_EPADDR          0x81           
#define IMU_EPADDR          0x82         

#define USB_TIMEOUT         10000           //传输数据的时间延迟

WinUSBDriver::WinUSBDriver()
{
    camThread = new USBThread();
    imuThread = new USBThread();
    
    camThread->init(this,0);
    imuThread->init(this,1);
    
    libusb_init(&m_libusb_context);
    
    ctrl_buffer = (unsigned char *)malloc(1024);
    imu_buffer  = (unsigned char *)malloc(14);
}

WinUSBDriver::~WinUSBDriver()
{
    if(m_libusb_context){
        libusb_exit(m_libusb_context);
        m_libusb_context = NULL;
    }
    
    free(ctrl_buffer);
    free(imu_buffer);
}

int WinUSBDriver::open(int vid,int pid)
{
    
    int error = 0,i = 0;
    dev_handle = NULL;
    
    dev_handle = libusb_open_device_with_vid_pid(m_libusb_context, vid, pid);
    if (dev_handle == NULL) {
        DBG("fail to open usb device");
        goto open_fail;
    }
    else {
        DBG("Device Opened\n");
    }
    
    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
		DBG("Kernel Driver Active\n");
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
			DBG("Kernel Driver Detached!\n");
		}
	}
    
    
    ret = libusb_claim_interface(dev_handle,0);
    if(ret < 0){
        DBG("Fail to libusb_claim_interface");
    }

    is_open = true;
    camThread->start();
    imuThread->start();
    
    return 0;
    
    
    fail:    
    open_fail:
    init_fail:
    libusb_exit(m_libusb_context);
    return 1;
}

void WinUSBDriver::CamRecv(void)
{
    recv_index = 0;
    while(is_open)
    {
        //DBG("imu start recv");
        ret = libusb_bulk_transfer(dev_handle, CAM_EPADDR, (unsigned char *)(img.img+recv_index), RECV_LEN ,&camRecvLen,0);
        if(ret < 0)
        {
            DBG("cam recv error");
            emit disconnectSignals();
            break;
    
        }else{
            recv_index += camRecvLen;
            recv_count_1s += camRecvLen;
            if(recv_index >= RECV_LEN)
            {
                //DBG("recv frame ");
                frame_fps++;
                recv_index = 0;
                emit recvSignals(recv_buf_tmp,camRecvLen);
            }
        }
    }
}

void WinUSBDriver::IMURecv(void)
{
    

    while(is_open)
    {
        //DBG("imu start recv");
        ret = libusb_bulk_transfer(dev_handle, IMU_EPADDR, (unsigned char *)(imu_buffer), 14 ,&imuRecvLen,0);
        if(ret < 0)
        {
            DBG("imu recv error");
            emit disconnectSignals();
            break;
    
        }else{
            recv_count_1s += imuRecvLen;
            imu_hz++;
            emit imuSignals(imu_buffer);
        }
    }
}

void WinUSBDriver::send(QByteArray byte)
{
    if(is_open)
    {
        libusb_bulk_transfer(dev_handle,CTRL_EPADDR,(unsigned char *)byte.data(),byte.length(),NULL,0xFFFF);
    }
    
}

int WinUSBDriver::close(void)
{
    is_open = false;
    
    camThread->waitClose();
    imuThread->waitClose();

    libusb_release_interface(dev_handle, 0);

    libusb_close(dev_handle);

    DBG("usb close");
    
}

#define REQUEST_CAMERA_START    0xA0
#define REQUEST_CAMERA_STOP     0xA1

#define REQUEST_IMU_START       0xB0
#define REQUEST_IMU_STOP        0xB1

int WinUSBDriver::sendCtrl(char request,unsigned char *buffer,int len)
{
    if(is_open)
    {
        if(len == 0 || buffer == NULL)
        {
            ret = libusb_control_transfer(dev_handle,LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_IN
                                          ,request,0,0,ctrl_buffer,1,1000);
        }else{
            ret = libusb_control_transfer(dev_handle,LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_IN
                                          ,request,0,0,buffer,len,1000);
        }
            if(ret < 0)
            {
                DBG("libusb_control_transfer fail");
                return 1;
            }else{
                if(ctrl_buffer[0] == 'S')
                {
                    DBG("libusb_control_transfer success %c",ctrl_buffer[0]);
                    return 0;
                }
            }
      

    }
}

void WinUSBDriver::ctrlCamStart()
{
    recv_index = 0;
    sendCtrl(REQUEST_CAMERA_START,NULL,0);
}

void WinUSBDriver::ctrlCamStop()
{
    sendCtrl(REQUEST_CAMERA_STOP,NULL,0);
}

void WinUSBDriver::ctrlIMUStart()
{
    sendCtrl(REQUEST_IMU_START,NULL,0);
}

void WinUSBDriver::ctrlIMUStop()
{
    sendCtrl(REQUEST_IMU_STOP,NULL,0);
}



