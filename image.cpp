#include "image.h"
#include <stdio.h>
#include <stdlib.h>

#define IMG_WIDTH   640
#define IMG_HIGH    480
#define IMG_SIZE

Image::Image()
{
    width   = IMG_WIDTH;
    high    = IMG_HIGH;
    size    = IMG_WIDTH*IMG_HIGH;
    
    img     = (unsigned char *)malloc(width*high*2);
    img_rgb = (unsigned char *)malloc(width*high*3);
    index = 0;
}

bool Image::recv(unsigned char* data,int len)
{
    static int recv_count = 0;
    if(recv_count>= size)
    {
        return false;
    }
    
    if(index + len >= size)
    {
        memcpy(img+index,data,size - index);
        memcpy(img,data + (size - index),len - (size - index));
        index = len - (size - index);
    }else{
        memcpy(img+index,data,len);
        index+=len;
    }
    
    recv_count += len;
    
    if(recv_count >= size)
    {
        recv_count -= size;
        return true;
    }
    return false;
    
}

unsigned char Image::CONVERT_ADJUST(double tmp)  
{  
    return (unsigned char)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));  
}  

//YUV420P to RGB24  
void Image::CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)  
{  
    unsigned char *tmpbuf = (unsigned char *)malloc(nWidth*nHeight*3);  
    unsigned char Y,U,V,R,G,B;  
    unsigned char* y_planar,*u_planar,*v_planar;  
    int rgb_width , u_width;  
    rgb_width = nWidth * 3;  
    u_width = (nWidth >> 1);  
    int ypSize = nWidth * nHeight;  
    int upSize = (ypSize>>2);  
    int offSet = 0;  
  
    y_planar = yuv_src;  
    u_planar = yuv_src + ypSize;  
    v_planar = u_planar + upSize;  
  
    for(int i = 0; i < nHeight; i++)  
    {  
        for(int j = 0; j < nWidth; j ++)  
        {  
            // Get the Y value from the y planar  
            Y = *(y_planar + nWidth * i + j);  
            // Get the V value from the u planar  
            offSet = (i>>1) * (u_width) + (j>>1);  
            V = *(u_planar + offSet);  
            // Get the U value from the v planar  
            U = *(v_planar + offSet);  
  
            // Cacular the R,G,B values  
            R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));  
            G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));  
            B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));  
            
            offSet = rgb_width * i + j * 3;  
  
            rgb_dst[offSet] = B;  
            rgb_dst[offSet + 1] = G;  
            rgb_dst[offSet + 2] = R;  
        }  
    }  
    free(tmpbuf);  
}

/** 
 * Generate YUV420P gray scale bar. 
 * @param width    Width of Output YUV file. 
 * @param height   Height of Output YUV file. 
 * @param ymin     Max value of Y 
 * @param ymax     Min value of Y 
 * @param barnum   Number of bars 
 * @param url_out  Location of Output YUV file. 
 */  
int Image::simplest_yuv420_graybar(int width, int height,int ymin,int ymax,int barnum,char *url_out)
{  
    int barwidth;  
    float lum_inc;  
    unsigned char lum_temp;  
    int uv_width,uv_height;  
    FILE *fp=NULL;  
    unsigned char *data_y=NULL;  
    unsigned char *data_u=NULL;  
    unsigned char *data_v=NULL;  
    int t=0,i=0,j=0;  
  
    barwidth=width/barnum;  
    lum_inc=((float)(ymax-ymin))/((float)(barnum-1));  
    uv_width=width/2;  
    uv_height=height/2;  
  
    data_y=(unsigned char *)malloc(width*height);  
    data_u=(unsigned char *)malloc(uv_width*uv_height);  
    data_v=(unsigned char *)malloc(uv_width*uv_height);  
  
    if((fp=fopen(url_out,"wb+"))==NULL){  
        printf("Error: Cannot create file!");  
        return -1;  
    }  
  
    //Output Info 打印YUV分量 
    printf("Y, U, V value from picture's left to right:\n");  
    for(t=0;t<(width/barwidth);t++){  
        lum_temp=ymin+(char)(t*lum_inc);  
        printf("%3d, 128, 128\n",lum_temp);  
    }  
    //Gen Data 
    //存储Y分量
    for(j=0;j<height;j++){  
        for(i=0;i<width;i++){  
            t=i/barwidth;  
            lum_temp = ymin + (char)(t * lum_inc);  
            data_y[j*width+i]=lum_temp;  
        }  
    }  
    //存储U分量
    for(j=0;j<uv_height;j++){  
        for(i=0;i<uv_width;i++){  
            data_u[j*uv_width+i]=128;  
        }  
    }  
    //存储V分量
    for(j=0;j<uv_height;j++){  
        for(i=0;i<uv_width;i++){  
            data_v[j*uv_width+i]=128;  
        }  
    }  
    fwrite(data_y,width*height,1,fp);  
    fwrite(data_u,uv_width*uv_height,1,fp);  
    fwrite(data_v,uv_width*uv_height,1,fp);  
    fclose(fp);  
    free(data_y);  
    free(data_u);  
    free(data_v);  
    return 0;  
}