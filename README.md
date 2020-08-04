# OPENVIO_PC

## OPENVIO控制上位机。 

## **OPENVIO线上文档请点击下面链接** 

[OPENVIO线上文档](http://guanglundz.com/openvio)  

## 简介

OPENVIO 一款脱胎于OPENMV的智能摄像头。

在OPENMV硬件基础上增加了USB2.0芯片(USB3315）和IMU芯片（ICM20948），除却兼容OPENMV固件外，还可以将摄像头原始图像（未压缩图像）和IMU九轴数据高速传输至PC，可以作为SLAM单目IMU方案研究的低廉传感器方案（如港科大的VINS-MONO）.多种接口方便扩展更多功能，比如扩展超声波或激光模块后作为PX4光流模块使用（暂未实现 还在研发中）。

## 源码和资料

[OPENVIO源码](https://gitee.com/guanglunking/OPENVIO_BOARD)【开发环境：Keil5】  

[OPENVIO PC上位机](https://gitee.com/guanglunking/OPENVIO_PC)【开发环境：QT5.6.0 qt-opensource-windows-x86-mingw492-5.6.0】  

[OPENVIO ROS源码](https://gitee.com/guanglunking/OPENVIO_ROS)

[淘宝店铺](https://item.taobao.com/item.htm?id=615919130291)  

## 使用 

#### 介绍
 
![OPENVIO上位机](https://images.gitee.com/uploads/images/2020/0310/120626_ecb2ca4d_683968.png "Screenshot from 2020-03-10 11-04-51.png")
#### 功能
* 摄像头数据显示与设置
* IMU数据显示与设置

#### Linux编译问题解决

* GL/gl.h: No such file or directory
```
sudo apt-get install mesa-common-dev
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```
  

* GL/glut.h: No such file or directory

```
sudo apt-get install freeglut3-dev
```



