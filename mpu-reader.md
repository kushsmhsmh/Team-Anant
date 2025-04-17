
>This is a C implementation to interface the **MPU-6050 IMU** accelerometer & gyrometer sensor with the **BeagleBone Black** using the I2C protocol. It reads the accelerometer and gyrometer field values along the **X**, **Y**, and **Z** axes and prints them continuously.

---

## 📌 Features

- Initializes the MPU-6050 sensor using I2C.
    
- Continuously reads 3-axis accelerometer and 3-axis gyrometer data.
    
- Outputs values in terminal continually.

    

---

## 🧰 Prerequisites

- BeagleBone Black (with I2C enabled)
    
- MPU-6050 sensor
    
- Wiring between BBB and sensor
    
- GCC installed
    
- I2C tools (i2c-dev using libi2c0 package only)
    

---

## 🧠 Sensor I2C Details

|Property|Value|
|---|---|
|Default Address|`0x68`|
|Interface|I2C|
|Register Width|8-bit|
|Data Width|16-bit (2 x 8-bit)|

---

## ⚡ Wiring Guide

|MPU6050 Pin|BBB Pin Name|BBB Header Pin|
|---|---|---|
|GND|Ground|P9_1 or P9_2|
|VCC|3.3V/5V|P9_3 or P9_4|
|SCL|I2C2_SCL|P9_19|
|SDA|I2C2_SDA|P9_20|

Enable I2C2 via device tree overlays or config.

---

## 🧾 Code Breakdown

### 1. Header Files

```c
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
```

- Provides access to low-level I/O functions.
    
- `i2c-dev.h` provides constants and structures to interact with I2C devices in Linux.
    

---

### 2. Register Definitions

```c
#define Addr0 0x68 
#define PWR_MGMT 0x6B
```

- Sensor address and register mapping for configuring and reading data.
    

```c
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
```

- Data output registers (low byte and high byte for each axis for the accelerometer and gyrometer).
    
```c
#define I2C_BUS "/dev/i2c-2"
#define ACCEL_SCALE 16384.0
#define GYRO_SCALE 250
```

- Defines the address for the i2c bus in use(here we use i2c-2), the scale for both the accelerometer and the gyrometer which can be changed based on need by configuring the 0x1B(Gyrometer) and 0x1C(Accelerometer) registers.
---

### 3. beginTransmission Function

```c
int beginTransmission(int device_addr)
```

- Establishes communication with the sensor based on the specified filename of the usable I2C bus using the file descriptor(fd).

- Opens the I2C device file `/dev/i2c-2`.
    
- Sets the sensor's I2C address using `ioctl()`.

- Also does error handling in case the communication fails to happen.

---

### 4. Reading Data

```c
int readValues(int fd)
```

This function performs the following:

1. Writes the register addresses to the sensor.
    
2. Reads the corresponding byte (LSB then MSB).

3. Performs error handling in case there is an issue writing or reading to or from the sensor. This is useful in case of loose wiring, incorrect address, etc.
    
4. Combines the two bytes to form a signed 16-bit value.
    
5. Prints the value corresponding to each axis for the accelerometer and gyrometer.

---

### 5. Main Function

```c
int main()
```

  
```c
uint8_t buf[2] = {PWR_MGMT, 0x00}; //0x6B is the PWR_MGMT register address and 0x00 is the value to wake up the device
if (write(fd, buf, 2) != 2) {
    perror("Failed to write to the i2c bus.\n");
    close(fd);
    return -1;
}    
```

- Initializes the sensor.

```c
sleep(2);    
```
- Causes a delay and ensures data isn't written before the sensor has woken up.

```c
while(1){
    readValues(fd);
    sleep(1);
    }
```

- Reads the data and prints it to the terminal continually.

---

## 🛠 Build Instructions

```bash
gcc -o mpu-reader mpu-reader.c
```

---

## 🚀 Run

```bash
sudo ./mpu-reader
```

> You may need root permissions to access `/dev/i2c-2`.

---

## 📤 Sample Output

```
Accelerometer: X=0.094 Y=0.049 Z=-1.01
Gyroscope: X=0 Y=0 Z=0
Accelerometer: X=0.094 Y=0.049 Z=-1.01
Gyroscope: X=2 Y=1 Z=-2
```

---



