#ifndef CANE_GYRO_H
#define CANE_GYRO_H

#include "QMI8658.h"
#include "I2C.h"

float gyro_xyz[3] = {0, 0, 0};
float last_gyro_z = 0;

const struct QMI8658Config gyro_config = {
    .inputSelection = QMI8658_CONFIG_GYR_ENABLE,
    .gyrRange = QMI8658GyrRange_512dps,
    .gyrOdr = QMI8658GyrOdr_1000Hz,
};

void setup_gyro()
{
   SAFE_I2C_CALL({
      QMI8658_init();
      QMI8658_Config_apply(&gyro_config);
      QMI8658_enableSensors(QMI8658_CONFIG_GYR_ENABLE);
   });
}

void read_gyro()
{
   SAFE_I2C_CALL({
      QMI8658_read_gyro_xyz(gyro_xyz);
   });
}

#endif