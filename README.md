# OPENVIO_PC

#### 介绍
OPENVIO控制上位机。  
![image](https://gitee.com/guanglunking/OPENVIO_PC/Screenshot from 2020-03-10 11-04-51.png)

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



