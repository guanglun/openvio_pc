#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>

class Image : public QObject
{
    Q_OBJECT
public:
    unsigned char * img;
    unsigned char * img_rgb;
    int width,high,index,size;
    Image();
    bool recv(unsigned char* data,int len);
    void CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight);
    int simplest_yuv420_graybar(int width, int height,int ymin,int ymax,int barnum,char *url_out);
private:
    unsigned char CONVERT_ADJUST(double tmp);    
};

#endif // IMAGE_H
