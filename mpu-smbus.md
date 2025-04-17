>This is a C implementation to interface the **MPU-6050 IMU** accelerometer & gyrometer sensor with the **BeagleBone Black** using the I2C protocol. It reads the accelerometer and gyrometer field values along the **X**, **Y**, and **Z** axes and prints them continuously.

---

## 📌 Features

- Initializes the MPU-6050 sensor using SMBus.
    
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
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
```

- Provides access to low-level I/O functions.
    
- `i2c-dev.h` provides constants and structures to interact with I2C devices in Linux.

   
---

### 2. Register Definitions and helper functions

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


```c
#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0
#define I2C_SMBUS_BYTE_DATA 2
#define I2C_SMBUS 0x0720
```

|Define | Purpose | Where It's Used | Meaning |
|---|---|---|---|
|I2C_SMBUS_READ | Set read direction | args.read_write = ... | Read 1 byte from a register |
|I2C_SMBUS_WRITE | Set write direction | args.read_write = ... | Write 1 byte to a register |
|I2C_SMBUS_BYTE_DATA | Define transaction type | args.size = ... | Access 1-byte registers |
|I2C_SMBUS | ioctl operation code | ioctl(fd, I2C_SMBUS, &args) | Triggers the I2C transaction |


```c
union i2c_smbus_data {
    uint8_t byte;
    uint16_t word;
    uint8_t block[34];
};
```

- Serves as a flexible data container that can hold different types of I2C data, depending on the kind of SMBus operation being performed. The Linux I2C driver uses this structure when calling `ioctl()` with the `I2C_SMBUS` command.


```c
int i2c_smbus_read_byte_data(int file, uint8_t command)
```

- Reads a single byte of data from a given register on an I2C device. `file` specifies the file descriptor for the I2C device. `command` specifies the register address being read from. 

- It sets `read_write` to 1(or `I2C_SMBUS_READ`) to indicate a read operation then sets the register address and specifies the data size to be read(in our case 1 Byte(`I2C_SMBUS_BYTE_DATA`)). Then `ioctl` is called with all of the above packed into `args`.

- If the above operations succeed, the value is stored otherwise an error is returned.


```c
int i2c_smbus_write_byte_data(int file, uint8_t command, uint8_t value)
```

- Writes a single byte of data to a given register on an I2C device. `file` specifies the file descriptor for the I2C device. `command` specifies the register address being read from. `value` specifies the byte to be written into register.

- Fills `data.byte` with the necessary write value(`value`). It sets `read_write` to 0(or `I2C_SMBUS_WRITE`) to indicate a write operation then sets the register address(`command`) and specifies the data size to be written(in our case 1 Byte(`I2C_SMBUS_BYTE_DATA`)). Then `ioctl` is called with all of the above packed into `args`.

- If the above operations succeed, the value is stored otherwise an error is returned.



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
if (i2c_smbus_write_byte_data(fd, PWR_MGMT, 0x00) < 0) {
    perror("Power management write failed");
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
while(1) {
    if(read_values(fd) < 0) {
        printf("Failed to read values\n");
        break;
    }
    sleep(1);
}
```

- Reads the data and prints it to the terminal continually. Error handling performed.


---

## 🛠 Build Instructions

```bash
gcc -o mpu-smbus mpu-smbus.c
```

---

## 🚀 Run

```bash
sudo ./mpu-smbus
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



