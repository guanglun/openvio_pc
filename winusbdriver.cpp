#include "winusbdriver.h"
#define CTRL_EPADDR         0x01    
#define CAM_EPADDR          0x81           
#define IMU_EPADDR          0x82         

#define USB_TIMEOUT         10000           //传输数据的时间延迟

WinUSBDriver::WinUSBDriver()
{
    camThread = new USBThread();
    imuThread = new USBThread();
    
    camThread->init(this,"cam");
    imuThread->init(this,"imu");
    
    
    
    ctrl_buffer = (unsigned char *)malloc(1024);
    imu_buffer  = (unsigned char *)malloc(14);
    
    connect(this,SIGNAL(closeSignals()),this,SLOT(closeSlot()));
}

WinUSBDriver::~WinUSBDriver()
{
//    if(m_libusb_context){
//        libusb_exit(m_libusb_context);
//        m_libusb_context = NULL;
//    }
    
    free(ctrl_buffer);
    free(imu_buffer);
}

int WinUSBDriver::open(int vid,int pid)
{
    
    dev_handle = NULL;
    
    ret = libusb_init(&m_libusb_context);
    if(ret < 0){
        DBG("libusb init fail %d",ret);
        goto init_fail;
    }else{
        DBG("libusb init success");
    }
    
    dev_handle = libusb_open_device_with_vid_pid(m_libusb_context, vid, pid);
    if (dev_handle == NULL) {
        DBG("device open fail %d",ret);
        goto open_fail;
    }
    else {
        DBG("device open success");
    }
    
//    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
//		DBG("Kernel Driver Active\n");
//		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
//			DBG("Kernel Driver Detached!\n");
//		}
//	}
    
    
    ret = libusb_claim_interface(dev_handle,0);
    if(ret < 0){
        DBG("claim interface fail %d",ret);
        goto claim_fail;
    }else{
        DBG("claim interface success");
    }

    is_open = true;
    camThread->start();
    imuThread->start();
    
    return 0;
    
    
    claim_fail:    
    libusb_close(dev_handle);
    open_fail:
    libusb_exit(m_libusb_context);
    init_fail:
    return 1;
}

void WinUSBDriver::CamRecv(void)
{
    DBG("cam recv start");
    //recv_index = 0;
    while(is_open)
    {

        ret = libusb_bulk_transfer(dev_handle, CAM_EPADDR, (unsigned char *)(img.img+recv_index), RECV_LEN ,&camRecvLen,1000);
        if(ret < 0)
        {
            if(ret != -7)
            {
                DBG("cam recv error %d",ret);
                emit disconnectSignals();
                break;
            }

    
        }else{
            recv_index += camRecvLen;
            recv_count_1s += camRecvLen;
            if(recv_index >= RECV_LEN)
            {
                frame_fps++;
                recv_index = 0;
                emit recvSignals(recv_buf_tmp,camRecvLen);
            }
        }
    }
    DBG("cam recv exit");
}

void WinUSBDriver::IMURecv(void)
{
    DBG("imu recv start");
    while(is_open)
    {

        ret = libusb_bulk_transfer(dev_handle, IMU_EPADDR, (unsigned char *)(imu_buffer), 14 ,&imuRecvLen,1000);
        if(ret < 0)
        {
            
            if(ret != -7)
            {
                DBG("imu recv error %d",ret);
                emit disconnectSignals();
                break;
            }
    
        }else{
            recv_count_1s += imuRecvLen;
            imu_hz++;
            emit imuSignals(imu_buffer);
        }
    }
    DBG("imu recv exit");
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
    emit closeSignals();
}

void WinUSBDriver::closeSlot(void)
{
    if(is_open)
    {
        is_open = false;
        
        DBG("closeSlot");
        
        libusb_release_interface(dev_handle, 0);
        
        //libusb_close(dev_handle);
        
        camThread->waitClose();
        DBG("camThread->waitClose()");
        
        imuThread->waitClose();
        DBG("imuThread->waitClose()");
        
        //libusb_release_interface(dev_handle, 0);
    
        libusb_close(dev_handle);
        libusb_exit(m_libusb_context);
        DBG("usb close");
    }    
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



