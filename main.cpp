#include "mbed.h"
#include "mbed-mpu6050/MPU6050.h"
#include "mbed-scope/scope.h"

I2C i2c(I2C_SDA, I2C_SCL);
MPU6050 imu(&i2c);
Scope scope(6);


// main() runs in its own thread in the OS
int main()
{
    printf("Starting...\n\r");

    i2c.frequency(400000); // 400 kHz

    ThisThread::sleep_for(200ms); // Let connection settle
    
    uint8_t whoami = imu.readByte(WHO_AM_I_MPU6050);

    if (whoami != 0x68) {
        printf("Invalid device...\n\r");
        return 1; // Correct device was not found
    }

    imu.setAScale(AFS_16G);
    imu.setGScale(GFS_2000DPS);

    imu.reset(); // Reset registers to default in preparation for device calibration
    imu.calibrate(); // Calibrate gyro and accelerometers, load biases in bias registers
    imu.init();

    float a[3] = {0.0f}, g[3] = {0.0f}, temp = 0.0f;

    while (true) {

        // If data ready bit set, all data registers have new data
        if (imu.readByte(INT_STATUS) & 0x01)    // check if data ready interrupt
        {
            imu.readData(a, g, &temp);

            scope.set(0, a[0]);
            scope.set(1, a[1]);
            scope.set(2, a[2]);
            scope.set(3, g[0]);
            scope.set(4, g[1]);
            scope.set(5, g[2]);
        }

        scope.send();

        ThisThread::sleep_for(4ms); // 250 Hz
    }
}
