#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>

#define IMG_MAX_WIDTH   752
#define IMG_MAX_HIGH    480

class Image : public QObject
{
    Q_OBJECT
public:
    unsigned char img[IMG_MAX_WIDTH*IMG_MAX_HIGH*2];
    unsigned char img_rgb[IMG_MAX_WIDTH*IMG_MAX_HIGH*3];
    int width,high,index,size;
    Image();
    void setImgSize(uint8_t frame_size_num);
    bool recv(unsigned char* data,int len);
    void CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight);
    int simplest_yuv420_graybar(int width, int height,int ymin,int ymax,int barnum,char *url_out);
private:
    unsigned char CONVERT_ADJUST(double tmp);    
};

#endif // IMAGE_H
