#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define Addr0 0x68 //device driver address when 0
#define PWR_MGMT 0x6B //power management register
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define GYRO_XOUT_H	0x43
#define GYRO_XOUT_L	0x44
#define GYRO_YOUT_H	0x45
#define GYRO_YOUT_L	0x46
#define GYRO_ZOUT_H	0x47
#define GYRO_ZOUT_L	0x48
#define filename "/dev/i2c-2"
#define ACCEL_SCALE 16384.0 // scale factor for accelerometer
#define GYRO_SCALE 250 // scale factor for gyroscope

//pin number for gnd is 1, 2 in P9
//pin number for vcc is 3, 4 in P9
//pin number for scl is 19
//pin number for sda is 20
//pin number for AD0 is 9 (lite)

int beginTransmission(int device_addr){
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the i2c bus\n");
        return -1;
    }
    printf("%s\n", filename);
    if (ioctl(fd, I2C_SLAVE, Addr0) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.\n");
        close(fd);
        return -1;
    }
    return fd;
}

int readValues(int fd){
    char AddrList[12];
    AddrList[0] = ACCEL_XOUT_H; //accelerometer register address
    AddrList[1] = ACCEL_XOUT_L; //accelerometer register address
    AddrList[2] = ACCEL_YOUT_H; //accelerometer register address
    AddrList[3] = ACCEL_YOUT_L; //accelerometer register address
    AddrList[4] = ACCEL_ZOUT_H; //accelerometer register address
    AddrList[5] = ACCEL_ZOUT_L; //accelerometer register address
    AddrList[6] = GYRO_XOUT_H; //gyroscope register address
    AddrList[7] = GYRO_XOUT_L; //gyroscope register address
    AddrList[8] = GYRO_YOUT_H; //gyroscope register address
    AddrList[9] = GYRO_YOUT_L; //gyroscope register address
    AddrList[10] = GYRO_ZOUT_H; //gyroscope register address
    AddrList[11] = GYRO_ZOUT_L; //gyroscope register address

    uint8_t values[12];
    for (int i = 0; i < 12; i++){
        if (write(fd, &AddrList[i], 1) != 1) {
            printf("write reg error\n");
            return -1;
        }
        if (read(fd, &values[i], 1) != 1) {
            perror("read reg error\n");
            return -1;
        }
    }
    int16_t ax = (int16_t)((values[0] << 8) | values[1]); // combine the two bytes to get the accelerometer x value
    int16_t ay = (int16_t)((values[2] << 8) | values[3]); // combine the two bytes to get the accelerometer y value
    int16_t az = (int16_t)((values[4] << 8) | values[5]); // combine the two bytes to get the accelerometer z value
    int16_t gx = (int16_t)((values[6] << 8) | values[7]); // combine the two bytes to get the gyroscope x value
    int16_t gy = (int16_t)((values[8] << 8) | values[9]); // combine the two bytes to get the gyroscope y value
    int16_t gz = (int16_t)((values[10] << 8) | values[11]); // combine the two bytes to get the gyroscope z value
    
    printf("Accelerometer: X=%f Y=%f Z=%f\n", (ax/ACCEL_SCALE), (ay/ACCEL_SCALE), (az/ACCEL_SCALE));
    printf("Gyroscope: X=%d Y=%d Z=%d\n", (gx/GYRO_SCALE), (gy/GYRO_SCALE), (gz/GYRO_SCALE));
    return 0;        
    
}

int main() {
    int fd = beginTransmission(0x68); // 0x68 is the I2C address of the MPU6050
    
    system("i2cdetect -r -y 2"); // to check the address of the device
    system("i2cdump -y 2 0x68"); // to check the registers of the device
    //system("i2cset -y 2 0x68 0x6B 0x00"); // to wake up the device and write 0 to the PWR_MGMT Register using system call
    
    uint8_t buf[2] = {PWR_MGMT, 0x00}; //0x6B is the PWR_MGMT register address and 0x00 is the value to wake up the device
    if (write(fd, buf, 2) != 2) {
        perror("Failed to write to the i2c bus.\n");
        close(fd);
        return -1;
    }

    sleep(2);
    
    while(1){
        readValues(fd);
        sleep(1);
    }
    close(fd);
    return 0;
}
