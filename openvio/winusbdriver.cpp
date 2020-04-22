#include "winusbdriver.h"
#include <QThread>

#define CTRL_EPADDR 0x01
#define CAM_EPADDR 0x81
#define IMU_EPADDR 0x82

#define USB_TIMEOUT 10000 //传输数据的时间延迟

#define IMU_PACKAGE_SIZE 24
#define IMU_PACKAGE_NUM  100

WinUSBDriver::WinUSBDriver()
{
    camThread = new USBThread();
    imuThread = new USBThread();

    camThread->init(this, "cam");
    imuThread->init(this, "imu");

    ctrl_buffer = (unsigned char *)malloc(1024);
    imu_buffer = (unsigned char *)malloc(IMU_PACKAGE_SIZE*IMU_PACKAGE_NUM);

    camStatus = SENSOR_STATUS_STOP;
    imuStatus = SENSOR_STATUS_STOP;

    connect(this, SIGNAL(closeSignals()), this, SLOT(closeSlot()));
    connect(this, SIGNAL(openSignals(int,int)), this, SLOT(openSlot(int,int)));
}

WinUSBDriver::~WinUSBDriver()
{
    free(ctrl_buffer);
    free(imu_buffer);
}

void WinUSBDriver::open(int vid, int pid)
{
    emit openSignals(vid,pid);
}

void WinUSBDriver::openSlot(int vid, int pid)
{
    int close_try_cnt = 0;
    dev_handle = NULL;

    ret = libusb_init(&m_libusb_context);
    if (ret < 0)
    {
        mlog->show("libusb init fail " + ret);
        goto init_fail;
    }
    else
    {
        mlog->show("libusb init success");
    }

    dev_handle = libusb_open_device_with_vid_pid(m_libusb_context, vid, pid);
    if (dev_handle == NULL)
    {
        mlog->show("device open fail " + ret);
        goto open_fail;
    }
    else
    {
        mlog->show("device open success");
    }

    //    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
    //		DBG("Kernel Driver Active\n");
    //		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
    //			DBG("Kernel Driver Detached!\n");
    //		}
    //	}

    ret = libusb_claim_interface(dev_handle, 0);
    if (ret < 0)
    {
        mlog->show("claim interface fail " + ret);
        goto claim_fail;
    }
    else
    {
        mlog->show("claim interface success");
    }
    
    is_open = true;
    
    
    close_try_cnt = 0;
    while(ctrlCamStop()!= 0 && close_try_cnt<4)
    {
        close_try_cnt++;
    }
    
    close_try_cnt = 0;
    while(ctrlIMUStop()!= 0 && close_try_cnt<4)
    {
        close_try_cnt++;
    }
    
    
    camThread->start();
    imuThread->start();

    mlog->show("open success");
    emit sendStatusSignals(USB_MSG_OPEN_SUCCESS);

    return;
    
close_fail:
    is_open = false;
claim_fail:
    libusb_close(dev_handle);
open_fail:
init_fail:
    mlog->show("open fail");
    emit sendStatusSignals(USB_MSG_OPEN_FAIL);
    return;
}

void WinUSBDriver::CamRecv(void)
{
    DBG("cam recv start");

    int recv_head_status = 0;
    int findRet = 0;
    FindStr findStr;
    uint8_t head_tmp[1024];
    findStr.config((unsigned char *)"CAMERA", 6);
    while (is_open)
    {
        if (recv_head_status == 0)
            ret = libusb_bulk_transfer(dev_handle, CAM_EPADDR, (unsigned char *)(head_tmp), 1024, &camRecvLen, 1000);
        else
            ret = libusb_bulk_transfer(dev_handle, CAM_EPADDR, (unsigned char *)(img.img + recv_index), 512*1024, &camRecvLen, 1000);
        if (ret < 0)
        {
            if (ret != -7)
            {
                DBG("cam recv error %d", ret);
                emit disconnectSignals();
                break;
            }else{
                //DBG("cam recv time out");
            }
        }
        else if(camStatus == SENSOR_STATUS_RUNNING)
        {
            //DBG("cam recv %d %d",camRecvLen,recv_index);
            recv_count_1s += camRecvLen;

            if (recv_head_status == 0)
            {
                findRet = findStr.input(head_tmp, camRecvLen);
                if (findRet > 0)
                {
                    recv_head_status = 1;
                }
            }
            else
            {
                recv_index += camRecvLen;
                if (recv_index >= (img.size*img.gs_bpp))
                {
                    frame_fps++;
                    recv_index = 0;
                    recv_head_status = 0;
                    emit recvSignals(img.img, camRecvLen);
                }
            }
        }
        
        
    }
    DBG("cam recv exit");
}

void WinUSBDriver::IMURecv(void)
{
    DBG("imu recv start");

    int recv_head_status = 0;
    int findRet = 0;
    FindStr findStr;
    findStr.config((unsigned char *)"ICMIMU", 6);
    uint8_t head_tmp[1024];
    uint32_t index = 0;

//    static uint32_t t1,t1_old;
//    static uint16_t t2,t2_old;
//    static boolean is_first = true;

    while (is_open)
    {   
//        if (recv_head_status == 0)
//            ret = libusb_bulk_transfer(dev_handle, IMU_EPADDR, (unsigned char *)(head_tmp), 1024, &imuRecvLen, 1000);
//        else
            ret = libusb_bulk_transfer(dev_handle, IMU_EPADDR, (unsigned char *)(imu_buffer+index), IMU_PACKAGE_SIZE, &imuRecvLen, 10);
        if (ret < 0)
        {

            if (ret != -7)
            {
                DBG("imu recv error %d", ret);
                emit disconnectSignals();
                break;
            }else{
                //DBG("imu recv time out");
            }
        }
        else
        {
            recv_count_1s += imuRecvLen;

//            if (recv_head_status == 0)
//            {
//                findRet = findStr.input(head_tmp, imuRecvLen);
//                if (findRet > 0)
//                {
//                    recv_head_status = 1;
//                }
//            }
//            else
//            {
//                recv_head_status = 0;
//                imu_hz++;
                 if(imuRecvLen == IMU_PACKAGE_SIZE)
                 {

//                     t1 = (uint32_t)(imu_buffer[0]<<24);
//                     t1 |= (uint32_t)(imu_buffer[1]<<16);
//                     t1 |= (uint32_t)(imu_buffer[2]<<8);
//                     t1 |= (uint32_t)(imu_buffer[3]<<0);

//                     t2 = (uint16_t)(imu_buffer[4]<<8);
//                     t2 |= (uint16_t)(imu_buffer[5]<<0);

//                     DBG("%d\t%d",t1,t2);

                     emit imuSignals(imu_buffer+index);
                     index+=IMU_PACKAGE_SIZE;
                     if(index >= (IMU_PACKAGE_SIZE*IMU_PACKAGE_NUM))
                     {
                         index = 0;
                     }
                 }
//            }
        }

        
    }
    DBG("imu recv exit");
}

void WinUSBDriver::send(QByteArray byte)
{
    if (is_open)
    {
        libusb_bulk_transfer(dev_handle, CTRL_EPADDR, (unsigned char *)byte.data(), byte.length(), NULL, 0xFFFF);
    }
}

int WinUSBDriver::close(void)
{
    emit closeSignals();
}

void WinUSBDriver::closeSlot(void)
{
    int close_try_cnt = 0;
    
    if (is_open)
    {
        close_try_cnt = 0;
        while(ctrlCamStop()!=0 && close_try_cnt>2)
        {
            close_try_cnt++;
            mlog->show("close cam fail");

        }
        
        close_try_cnt = 0;
        while(ctrlIMUStop()!=0 && close_try_cnt>2)
        {
            close_try_cnt++;
            mlog->show("close imu fail");
        }

        is_open = false;

        camThread->waitClose();
        DBG("camThread->waitClose()");

        imuThread->waitClose();
        DBG("imuThread->waitClose()");
        
        DBG("closeSlot");

        //libusb_release_interface(dev_handle, 0);

//        libusb_close(dev_handle);

//        libusb_release_interface(dev_handle, 0);

        libusb_close(dev_handle);
        libusb_exit(m_libusb_context);

        mlog->show("close success");
        emit sendStatusSignals(USB_MSG_CLOSE_SUCCESS);

    }
}

#define REQUEST_CAMERA_START 0xA0
#define REQUEST_CAMERA_STOP 0xA1
#define REQUEST_CAMERA_SET_FRAME_SIZE_NUM     	0xA2

#define REQUEST_IMU_START 0xB0
#define REQUEST_IMU_STOP 0xB1

int WinUSBDriver::sendCtrl(char request, uint16_t wValue,uint16_t wIndex,unsigned char *buffer)
{
    if (is_open)
    {

        ret = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_IN, request, wValue, wIndex, buffer, 128, 1000);

        if (ret < 0)
        {
            DBG("libusb_control_transfer fail");
            return -1;
        }
        else
        {
            DBG("libusb_control_transfer success %d %c",ret,buffer[0]);

            return ret;

        }
    }

    return -1;
}

int WinUSBDriver::ctrlCamStart()
{
    uint8_t ret = 0;
    recv_index = 0;
    
    ret = sendCtrl(REQUEST_CAMERA_START, 0,0,ctrl_buffer);
    if ((ret >= 0) && (ctrl_buffer[0] == 'S'))
    {
        camStatus = SENSOR_STATUS_RUNNING;
        recv_index = 0;
        DBG("ret:%d\t id:%d \t bpp:%d\t size:%d\t pixformat:%d",ret,ctrl_buffer[1],ctrl_buffer[3],ctrl_buffer[2],ctrl_buffer[4]);
        cam_id = ctrl_buffer[1];
        img.gs_bpp = ctrl_buffer[3];
        img.setImgSize(ctrl_buffer[2]);
        pixformat = (pixformat_t)ctrl_buffer[4];
        
        return 0;
    }
    
    return -1;
}

int WinUSBDriver::ctrlCamStop()
{
    uint8_t ret = 0;
                                        
    ret = sendCtrl(REQUEST_CAMERA_STOP, 0,0,ctrl_buffer);
    if ((ret > 0) && (ctrl_buffer[0] == 'S'))
    {
        camStatus = SENSOR_STATUS_STOP;
        return 0;
    }
    
    return -1;
}

int WinUSBDriver::ctrlIMUStart()
{
    uint8_t ret = 0;
                                        
    ret = sendCtrl(REQUEST_IMU_START, 0,0,ctrl_buffer);
    if ((ret > 0) && (ctrl_buffer[0] == 'S'))
    {
        imuStatus = SENSOR_STATUS_RUNNING;
        return 0;
    }
    return -1;
}

int WinUSBDriver::ctrlIMUStop()
{

    uint8_t ret = 0;
                                            
    ret = sendCtrl(REQUEST_IMU_STOP, 0,0,ctrl_buffer);
    if ((ret > 0) && (ctrl_buffer[0] == 'S'))         
    {
        imuStatus = SENSOR_STATUS_STOP;
        return 0;
    }
    return -1;
}

int WinUSBDriver::ctrlCamSetFrameSizeNum(uint16_t num)
{

    uint8_t ret = 0;
                                            
    ret = sendCtrl(REQUEST_CAMERA_SET_FRAME_SIZE_NUM, num,0,ctrl_buffer);
    if ((ret >= 0) && (ctrl_buffer[0] == 'S'))         
    {
        DBG("ret:%d\t id:%d \t bpp:%d\t size:%d\t pixformat:%d",ret,ctrl_buffer[1],ctrl_buffer[3],ctrl_buffer[2],ctrl_buffer[4]);
        cam_id = ctrl_buffer[1];
        img.gs_bpp = ctrl_buffer[3];
        img.setImgSize(ctrl_buffer[2]);
        pixformat = (pixformat_t)ctrl_buffer[4];
        
        return 0;
    }
    return -1;
}

