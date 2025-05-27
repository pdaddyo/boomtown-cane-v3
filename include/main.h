

#define UI_VERSION "4.1.0"

#ifdef ESPS3_1_69

// screen configs
#define WIDTH 240
#define HEIGHT 280
#define OFFSET_X 0
#define OFFSET_Y 20
#define RGB_ORDER true

// touch
#define I2C_SDA 11
#define I2C_SCL 10
#define TP_INT 14
#define TP_RST 13

// display
#define SPI SPI2_HOST

#define SCLK 6
#define MOSI 7
#define MISO -1
#define DC 4
#define CS 5
#define RST 8

#define BL 15

#define BUZZER 33

#define MAX_FILE_OPEN 20

#define CS_CONFIG CS_240x296_191_RTF

#else

// screen configs
#define WIDTH 240
#define HEIGHT 240
#define OFFSET_X 0
#define OFFSET_Y 0
#define RGB_ORDER false

// touch
#define I2C_SDA 21
#define I2C_SCL 22
#define TP_INT 14
#define TP_RST 5

// display
#define SPI VSPI_HOST

#define SCLK 18
#define MOSI 23
#define MISO -1
#define DC 4
#define CS 15
#define RST 13

#define BL 2

#define BUZZER -1

#define MAX_FILE_OPEN 10

#endif
