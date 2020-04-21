#include "imu.h"
#include <QtMath>
#include "workspace.h"

#define RtA 57.324841f
#define AtR 0.0174533f
#define Acc_G 0.0011963f
#define Gyro_G 0.0610351f
#define Gyro_Gr 0.0010653f
#define FILTER_NUM 5

//采样速度
#define Kp 1.000f    // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.001f    // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.0025f // half the sample period

IMU::IMU()
{
    setting->getIMUOffset(&ACC_OFFSET,&GYRO_OFFSET);
    startCalibration();
}

void IMU::startCalibration(void)
{
    DBG("start calibration");
    isYawCalStart = false;
    isGyroCalStart = false;
    isAccCalStart = false;
}

void IMU::calibration(T_int16_xyz *acc,T_int16_xyz *gyro)
{
    static T_int32_xyz tempg = {0,0,0},tempa = {0,0,0};
    static unsigned char cnt_g = 0,cnt_a = 0;
    
    if(isGyroCalStart)
    {
        if(cnt_g==0)
		{
			GYRO_OFFSET.X=0;
			GYRO_OFFSET.Y=0;
			GYRO_OFFSET.Z=0;
			tempg.X = 0;
			tempg.Y = 0;
			tempg.Z = 0;
			cnt_g = 1;
		}else{
            tempg.X += gyro->X;
            tempg.Y += gyro->Y;
            tempg.Z += gyro->Z;
            if(cnt_g == 200)
            {
                GYRO_OFFSET.X = tempg.X/cnt_g;
                GYRO_OFFSET.Y = tempg.Y/cnt_g;
                GYRO_OFFSET.Z = tempg.Z/cnt_g;
                setting->setGyroOffset(&GYRO_OFFSET);
                
                DBG("calibration gyr %d,%d,%d",GYRO_OFFSET.X,GYRO_OFFSET.Y,GYRO_OFFSET.Z);
                
                cnt_g = 0;
                isGyroCalStart = false;
            }else{
                cnt_g++;
            }
        }
		
    }
    
    if(isAccCalStart)
    {
        if(cnt_a == 0)
		{
			ACC_OFFSET.X = 0;
			ACC_OFFSET.Y = 0;
			ACC_OFFSET.Z = 0;
			tempa.X = 0;
			tempa.Y = 0;
			tempa.Z = 0;
			cnt_a = 1;
		}else{ 
            tempa.X += acc->X;
            tempa.Y += acc->Y;
            //tempa.Z += acc->Z;
            if(cnt_a == 200)
            {
                ACC_OFFSET.X = tempa.X/cnt_a;
                ACC_OFFSET.Y = tempa.Y/cnt_a;
                //ACC_OFFSET.Z = tempa.Z/cnt_a;
                
                setting->setAccOffset(&ACC_OFFSET);
                
                DBG("calibration acc %d,%d,%d",ACC_OFFSET.X,ACC_OFFSET.Y,ACC_OFFSET.Z);
                cnt_a = 0;
                isAccCalStart = false;
            }else{
                cnt_a++;
            }
        }

    }
}

void IMU::recvData(unsigned char *imu_data,T_float_angle *angle,float time)
{
    Acc.X = (short)((imu_data[0]<<8)|imu_data[1]);// - ACC_OFFSET.X;
    Acc.Y = (short)((imu_data[2]<<8)|imu_data[3]);// - ACC_OFFSET.Y;
    Acc.Z = (short)((imu_data[4]<<8)|imu_data[5]);// - ACC_OFFSET.Z;
    
    Gyr.X = (short)((imu_data[6]<<8)|imu_data[7]);//   - GYRO_OFFSET.X;
    Gyr.Y = (short)((imu_data[8]<<8)|imu_data[9]);// - GYRO_OFFSET.Y;
    Gyr.Z = (short)((imu_data[10]<<8)|imu_data[11]);// - GYRO_OFFSET.Z;
    
    //temp = (short)((imu_data[12]<<8)|imu_data[13]);

    calibration(&Acc,&Gyr);
    prepareData(&Acc,&Acc_AVG);
    update(&Gyr,&Acc_AVG,angle,time);
    
}

void IMU::prepareData(T_int16_xyz *acc_in, T_int16_xyz *acc_out)
{
  static uint8_t filter_cnt = 0;
  static int16_t ACC_X_BUF[FILTER_NUM], ACC_Y_BUF[FILTER_NUM], ACC_Z_BUF[FILTER_NUM];
  int32_t temp1 = 0, temp2 = 0, temp3 = 0;
  uint8_t i;

  ACC_X_BUF[filter_cnt] = acc_in->X;
  ACC_Y_BUF[filter_cnt] = acc_in->Y;
  ACC_Z_BUF[filter_cnt] = acc_in->Z;
  for (i = 0; i < FILTER_NUM; i++)
  {
    temp1 += ACC_X_BUF[i];
    temp2 += ACC_Y_BUF[i];
    temp3 += ACC_Z_BUF[i];
  }
  acc_out->X = temp1 / FILTER_NUM;
  acc_out->Y = temp2 / FILTER_NUM;
  acc_out->Z = temp3 / FILTER_NUM;
  filter_cnt++;
  if (filter_cnt == FILTER_NUM)
    filter_cnt = 0;
}

void IMU::update(T_int16_xyz *gyr, T_int16_xyz *acc, T_float_angle *angle,float time)
{
    static float acc_cal = 9.8f*8.0f/65535*2;

    float gx, gy, gz, ax, ay, az;

    ax = acc->X * acc_cal;
    ay = acc->Y * acc_cal;
    az = acc->Z * acc_cal;

    gx = gyr->X * (500.0 / 65536.0) * (M_PI / 180.0);
    gy = gyr->Y * (500.0 / 65536.0) * (M_PI / 180.0);
    gz = gyr->Z * (500.0 / 65536.0) * (M_PI / 180.0);

    //DBG("%f\t%f\t%f\t%f\t%f\t%f\r\n",gx,gy,gz,ax,ay,az);

    float norm;
  //  float hx, hy, hz, bx, bz;
    float vx, vy, vz;// wx, wy, wz;
    float ex, ey, ez;

    // 先把这些用得到的值算好
    float q0q0 = q0*q0;
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
  //  float q0q3 = q0*q3;
    float q1q1 = q1*q1;
  //  float q1q2 = q1*q2;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    float q3q3 = q3*q3;

      if(ax*ay*az==0)
          return;

    norm = sqrt(ax*ax + ay*ay + az*az);       //acc数据归一化
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // estimated direction of gravity and flux (v and w)      估计重力方向和流量/变迁
    vx = 2*(q1q3 - q0q2);												//四元素中xyz的表示
    vy = 2*(q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors
    ex = (ay*vz - az*vy) ;                           					 //向量外积在相减得到差分就是误差
    ey = (az*vx - ax*vz) ;
    ez = (ax*vy - ay*vx) ;

    exInt = exInt + ex * Ki;								  //对误差进行积分
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // adjusted gyroscope measurements
    gx = gx + Kp*ex + exInt;					   							//将误差PI后补偿到陀螺仪，即补偿零点漂移
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;				   							//这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减

    // integrate quaternion rate and normalise						   //四元素的微分方程
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*time/2;
    q1 = q1 + (q0*gx + q2*gz - q3*gy)*time/2;
    q2 = q2 + (q0*gy - q1*gz + q3*gx)*time/2;
    q3 = q3 + (q0*gz + q1*gy - q2*gx)*time/2;

    // normalise quaternion
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

  angle->yaw = qAtan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3;

  angle->pit = qAsin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; ;                                  // pitch
  angle->rol = -qAtan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
  
  if(isYawCalStart)
  {
      isYawCalStart = false;
      angle->yaw = 0;
  }
  
}
