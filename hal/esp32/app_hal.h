#ifndef DRIVER_H
#define DRIVER_H

#if defined(ESPS3_1_69) || defined(ESPS3_1_28)
#define ENABLE_APP_QMI8658C
#endif

#ifdef __cplusplus
extern "C"
{
#endif
   void leds_setup();
   void leds_loop();
   void update_gyro();
   float get_gyro_x();
   float get_gyro_y();
   float get_gyro_z();
   void hal_setup(void);
   void hal_loop(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DRIVER_H*/
