#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define Addr0 0x68
#define PWR_MGMT 0x6B
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
#define I2C_BUS "/dev/i2c-2"
#define ACCEL_SCALE 16384.0
#define GYRO_SCALE 250

#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0
#define I2C_SMBUS_BYTE_DATA 2
#define I2C_SMBUS 0x0720

union i2c_smbus_data {
    uint8_t byte;
    uint16_t word;
    uint8_t block[34];
};

int i2c_smbus_read_byte_data(int file, uint8_t command) {
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    args.read_write = I2C_SMBUS_READ;
    args.command = command;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    if (ioctl(file, I2C_SMBUS, &args) < 0)
        return -1;
    else
        return data.byte;
}

int i2c_smbus_write_byte_data(int file, uint8_t command, uint8_t value) {
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    data.byte = value;
    args.read_write = I2C_SMBUS_WRITE;
    args.command = command;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    return ioctl(file, I2C_SMBUS, &args);
}


int begin_transmission(int device_addr) {
    int fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) {
        perror("Failed to open i2c bus");
        return -1;
    }
    
    if (ioctl(fd, I2C_SLAVE, Addr0) < 0) {
        perror("Failed to acquire bus access");
        close(fd);
        return -1;
    }
    return fd;
}

int read_values(int fd) {
    char AddrList[12] = {ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_YOUT_H, ACCEL_YOUT_L, ACCEL_ZOUT_H, ACCEL_ZOUT_L, GYRO_XOUT_H, GYRO_XOUT_L, GYRO_YOUT_H, GYRO_YOUT_L, GYRO_ZOUT_H, GYRO_ZOUT_L}; 
    uint8_t values[12];
    
    for (int i = 0; i < 12; i++) {
        int result = i2c_smbus_read_byte_data(fd, AddrList[i]);
        if (result < 0) {
            perror("SMBus read failed");
            return -1;
        }
        values[i] = (uint8_t)result;
    }

    int16_t ax = (int16_t)((values[0] << 8) | values[1]); // combine the two bytes to get the accelerometer x value
    int16_t ay = (int16_t)((values[2] << 8) | values[3]); // combine the two bytes to get the accelerometer y value
    int16_t az = (int16_t)((values[4] << 8) | values[5]); // combine the two bytes to get the accelerometer z value
    int16_t gx = (int16_t)((values[6] << 8) | values[7]); // combine the two bytes to get the gyroscope x value
    int16_t gy = (int16_t)((values[8] << 8) | values[9]); // combine the two bytes to get the gyroscope y value
    int16_t gz = (int16_t)((values[10] << 8) | values[11]); // combine the two bytes to get the gyroscope z value
    
    printf("Accelerometer: X=%f Y=%f Z=%f\n", ax/ACCEL_SCALE, ay/ACCEL_SCALE, az/ACCEL_SCALE);
    printf("Gyroscope: X=%d Y=%d Z=%d\n", gx/GYRO_SCALE, gy/GYRO_SCALE, gz/GYRO_SCALE);
    return 0;
}

int main() {
    int fd = begin_transmission(0x68);

    // Wake up sensor (write 0 to PWR_MGMT register)
    if (i2c_smbus_write_byte_data(fd, PWR_MGMT, 0x00) < 0) {
        perror("Power management write failed");
        close(fd);
        return -1;
    }

    sleep(2);    

    while(1) {
        if(read_values(fd) < 0) {
            printf("Failed to read values\n");
            break;
        }
        sleep(1);
    }
    
    close(fd);
    return 0;

}
