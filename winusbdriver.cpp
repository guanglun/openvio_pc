#include "winusbdriver.h"



WinUSBDriver::WinUSBDriver()
{
    usb_thread = new USBThread();
    
    libusb_init(&m_libusb_context);
}

WinUSBDriver::~WinUSBDriver()
{
    if(m_libusb_context){
        libusb_exit(m_libusb_context);
        m_libusb_context = NULL;
    }
    
}

#define USB_VID         0x07E4          //USB的产商ID
#define USB_PID         0x07E4          //USB的产品ID

#define EP0ADDR         0x01            //Write端口0地址，通道0
#define EP1ADDR         0x81            //Read 端口1地址，通道1
#define EP2ADDR         0x02            //Write端口2地址，通道2
#define EP3ADDR         0x86            //Read 端口3地址，通道3

#define USB_TIMEOUT     10000           //传输数据的时间延迟




void LIBUSB_CALL WinUSBDriver::completeCallback(libusb_transfer *xfer)
{
    static int recv_count;
    WinUSBDriver *driver = (WinUSBDriver *)xfer->user_data;
    
    switch(xfer->status)
    {
        case LIBUSB_TRANSFER_COMPLETED:
        

            if(xfer->actual_length > 0){
                
                if(recv_count < driver->img.size)
                {
                driver->recv_len = xfer->actual_length;
                driver->recv_count_1s += xfer->actual_length;
                
                DBG("xfer->buffer %02X",xfer->buffer);
                //xfer->buffer = xfer->buffer + xfer->actual_length;
                recv_count += xfer->actual_length;
                DBG("xfer->buffer %02X",xfer->buffer);
                DBG("recv %d",recv_count);
                
                if(recv_count >= driver->img.size)
                {
                    emit driver->recvSignals(driver->recv_buf_tmp,driver->recv_len);
                }
                }
//                if(driver->img.recv(xfer->buffer,xfer->actual_length))
//                {
//                    emit driver->recvSignals(driver->recv_buf_tmp,driver->recv_len);
//                }
                
                
                
                //memcpy(img.img+index,xfer->buffer,xfer->actual_length);
                //emit driver->recvSignals(driver->recv_buf_tmp,driver->recv_len);
            }
        
                            
        break;
        case LIBUSB_TRANSFER_CANCELLED:
            DBG("LIBUSB_TRANSFER_CANCELLED");
            return;
            break;
        case LIBUSB_TRANSFER_NO_DEVICE:
            DBG("LIBUSB_TRANSFER_NO_DEVICE");
            break;
        case LIBUSB_TRANSFER_TIMED_OUT:
            DBG("LIBUSB_TRANSFER_TIMED_OUT");
            break;
        case LIBUSB_TRANSFER_ERROR:
            DBG("LIBUSB_TRANSFER_ERROR");
            break;
        case LIBUSB_TRANSFER_STALL:
            DBG("LIBUSB_TRANSFER_STALL");
            emit driver->disconnectSignals();
            
            break;
        case LIBUSB_TRANSFER_OVERFLOW:
            DBG("LIBUSB_TRANSFER_OVERFLOW");
            break;
        default:
        break;
    }

    if(driver->is_open)
    {
        int r = libusb_submit_transfer(xfer);
        if( r < 0 ){
            qDebug() << "fail to re-submit transfer";
            
        }    
    }

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
    
//    ret = libusb_get_device_list(m_libusb_context, &devs);
//    if (ret < 0)
//    {
//        DBG("get usb device list error when open");
//        error =1;
//        goto get_list_fail;
//    }else{
//        DBG("libusb_get_device_list success");
//    }
    
//    while ((dev = devs[i++]) != NULL)
//    {
//        //DBG("start to get device descriptor");
//        ret = libusb_get_device_descriptor(dev, &desc);
//        if (ret < 0) {
//            DBG("failed to get device descriptor");
//            error =1;
//            goto fail;
//        }else{
//            DBG( "Find VID:%02X PID:%02X",desc.idVendor,desc.idProduct);
//            if(desc.idVendor == vid && desc.idProduct == pid)
//            {
                
//                ret = libusb_open(dev, &dev_handle);
//                if (ret < 0)
//                {
//                    dev_handle = NULL;
//                    DBG("fail to open usb device");
//                    error =1;
//                    goto open_fail;
//                }else{
//                    DBG("libusb_open success");
//                    break;
//                }
//            }
//        }
//    }
//    libusb_free_device_list(devs, 1);
    
//    if(dev_handle == NULL)
//    {
//        DBG("fail find device");
//        error = 1;
//        goto find_device_fail;
//    }
    
//    ret = libusb_get_active_config_descriptor(dev, &cfg);
//    if (ret < 0){
//        DBG("Fail to get device config_descriptor");
//        error =1;
//        goto get_config_fail;
//    }else{
//        DBG("libusb_get_active_config_descriptor success");
//    }
    
//    for (int j = 0; j < cfg->bNumInterfaces; j++){
//        const struct libusb_interface_descriptor *interface_desc = (cfg->interface+j)->altsetting;
//        //DBG("bInterfaceNumber:%d",interface_desc->bInterfaceNumber);
//        //DBG("bNumEndpoints:%d",interface_desc->bNumEndpoints);
//    }
    
//    libusb_free_config_descriptor(cfg);
    
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
    
//    m_xfer = libusb_alloc_transfer(0);
    
//    libusb_fill_bulk_transfer(m_xfer,
//                        dev_handle,
//                        EP1ADDR, // Endpoint ID
//                        recv_buf,
//                        RECV_LEN,
//                        WinUSBDriver::completeCallback,
//                        this,
//                        0
//                        );
    
//    if(m_xfer){
//        ret = libusb_submit_transfer(m_xfer);
//        if( ret < 0 ){
//            DBG("fail to re-submit transfer");
//            libusb_free_transfer(m_xfer);
//            m_xfer = 0;
//            error =1;
//            goto fail;
//        }
//    }
    
    usb_thread->init(this);
    is_open = true;
    usb_thread->start();
    
    
    return 0;
    
    
    fail:    
    open_fail:
    init_fail:
    libusb_exit(m_libusb_context);
    return 1;
}

void WinUSBDriver::recv(void)
{
    recv_index = 0;
    while(is_open)
    {
        DBG("start recv");
        ret = libusb_bulk_transfer(dev_handle, EP1ADDR, (unsigned char *)(img.img+recv_index), RECV_LEN ,&recv_len,0);
        if(ret < 0)
        {
            DBG("recv error");
            emit disconnectSignals();
            break;
    
        }else{
            recv_index += recv_len;
            recv_count_1s += recv_len;
            if(recv_index >= RECV_LEN)
            {
                DBG("recv frame ");
                frame_fps++;
                recv_index = 0;
                emit recvSignals(recv_buf_tmp,recv_len);
            }
        }
    }
    
}

void WinUSBDriver::send(QByteArray byte)
{
    if(is_open)
    {
        libusb_bulk_transfer(dev_handle,EP0ADDR,(unsigned char *)byte.data(),byte.length(),NULL,0xFFFF);
    }
    
}

int WinUSBDriver::close(void)
{
    is_open = false;
    
    usb_thread->waitClose();


    libusb_release_interface(dev_handle, 0);

    libusb_close(dev_handle);

    DBG("usb close");
    
}


