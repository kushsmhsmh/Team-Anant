#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

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
 
//pin number for gnd is 1, 2 in P9
//pin number for vcc is 3, 4 in P9
//pin number for scl is 19
//pin number for sda is 20
//pin number for AD0 is 9 (lite)

int main() {
    int fd = open("/dev/i2c-2", O_RDWR); //open the i2c bus
    if (fd < 0) {
        perror("Failed to open the i2c bus");
        return -1;
    }
    int addr = Addr0; // I2C address of the device
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.\n");
        return -1;
    }
    system("i2cdetect -y 2"); // to check the address of the device
    system("i2cdump 2 0x68"); // to check the registers of the device
    system("i2cset -y 2 0x68 0x6B 0x00"); // to wake up the device and write 0 to the PWR_MGMT_1 Register
    int a = 0;
    printf("Do you want me? Press 1 if yes and once, 2 if yes and forever, 0 if no :(\n");
    scanf("%d", &a);
    // int16_t accel_x;
    // int16_t accel_y;
    // int16_t accel_z;
    if (a == 1) {
        // uint8_t Accel_X_H = system("i2cget -y 2 0x68 0x3B"); // to read the high byte data for the X Axis but this doesn't work because the system call value is not stored in the variable defined
        // uint8_t Accel_X_L = system("i2cget 2 0x68 0x3C"); // to read the low byte data for the X Axis
        // uint8_t Accel_Y_H = system("i2cget 2 0x68 0x3D"); // to read the high byte data for the Y Axis
        // uint8_t Accel_Y_L = system("i2cget 2 0x68 0x3E"); // to read the low byte data for the Y Axis
        // uint8_t Accel_Z_H = system("i2cget 2 0x68 0x3F"); // to read the high byte data for the Z Axis
        // uint8_t Accel_Z_L = system("i2cget 2 0x68 0x40"); // to read the low byte data for the Z Axis
        printf("Accel_X_H = %d\n", system("i2cget -y 2 0x68 0x3B")); // to print the X Axis high byte
        printf("Accel_X_L = %d\n", system("i2cget -y 2 0x68 0x3C")); // to print the X Axis low byte
        printf("Accel_Y_H = %d\n", system("i2cget -y 2 0x68 0x3D")); // to print the Y Axis high byte
        printf("Accel_Y_L = %d\n", system("i2cget -y 2 0x68 0x3E")); // to print the Y Axis low byte
        printf("Accel_Z_H = %d\n", system("i2cget -y 2 0x68 0x3F")); // to print the Z Axis high byte
        printf("Accel_Z_L = %d\n", system("i2cget -y 2 0x68 0x40")); // to print the Z Axis low byte
        }
    else if (a == 2){
        int b = 0;
        while (b == 0){
            printf("Do you want me to stop? Press 1 if yes and 0 if no :(\n");
            scanf("%d", &b);
            printf("Accel_X_H = %d\n", system("i2cget -y 2 0x68 0x3B"));
            printf("Accel_X_L = %d\n", system("i2cget -y 2 0x68 0x3C")); 
            printf("Accel_Y_H = %d\n", system("i2cget -y 2 0x68 0x3D")); 
            printf("Accel_Y_L = %d\n", system("i2cget -y 2 0x68 0x3E")); 
            printf("Accel_Z_H = %d\n", system("i2cget -y 2 0x68 0x3F")); 
            printf("Accel_Z_L = %d\n", system("i2cget -y 2 0x68 0x40")); 
        }
    }
    else {
        printf("I am not your friend\n");
        return 0;
    }
    // accel_x = (Accel_X_H << 8) | Accel_X_L; // to combine the high and low byte data for the X Axis
    // accel_y = (Accel_Y_H << 8) | Accel_Y_L; // to combine the high and low byte data for the Y Axis
    // accel_z = (Accel_Z_H << 8) | Accel_Z_L; // to combine the high and low byte data for the Z Axis
    
}
