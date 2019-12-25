#ifndef IMU_H
#define IMU_H


typedef struct{
				float rol;
				float pit;
				float yaw;
}T_float_angle;

typedef struct{
				float X;
				float Y;
				float Z;
}T_float_xyz;

typedef struct{
				signed short X;
				signed short Y;
				signed short Z;
}T_int16_xyz;

typedef struct{
				signed int X;
				signed int Y;
				signed int Z;
}T_int32_xyz;

class IMU
{
private:
    
    float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    // quaternion elements representing the estimated orientation
    float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error
    float AngleOffset_Yaw = 0,AngleOffset_Rol = 0,AngleOffset_Pit = 0;
    
    T_int16_xyz Acc,Gyr;
    T_int16_xyz Acc_AVG;
    bool isGyroCalStart = false,isAccCalStart = false,isYawCalStart = false;
    T_int16_xyz GYRO_OFFSET,ACC_OFFSET;
    
    void calibration(T_int16_xyz *acc,T_int16_xyz *gyro);
    void prepareData(T_int16_xyz *acc_in,T_int16_xyz *acc_out);
    void update(T_int16_xyz *gyr, T_int16_xyz *acc, T_float_angle *angle);
    
public:
    IMU();
    void startCalibration(void);
    void recvData(unsigned char *imu_data,T_float_angle *angle);

    
};

#endif // IMU_H
