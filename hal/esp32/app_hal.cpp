/*
   MIT License

  Copyright (c) 2023 Felix Biego

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ______________  _____
  ___  __/___  /_ ___(_)_____ _______ _______
  __  /_  __  __ \__  / _  _ \__  __ `/_  __ \
  _  __/  _  /_/ /_  /  /  __/_  /_/ / / /_/ /
  /_/     /_.___/ /_/   \___/ _\__, /  \____/
                              /____/

*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Timber.h>
#include <Preferences.h>
#include <Wire.h>
#include <Hardware.h>
#include <Arduino.h>
#include "app_hal.h"
#include "images.h"
#include "patterns.h"
#include "USB.h"

#include <lvgl.h>
#include "ui/ui.h"

#include "main.h"
#include "splash.h"

#include "FS.h"
#include "FFat.h"

#include "Qmi8658c.h"                // Include the library for QMI8658C sensor
#define QMI_ADDRESS 0x6B             // Define QMI8658C I2C address
#define QMI8658C_I2C_FREQUENCY 40000 // Define I2C frequency as 80kHz (in Hz)

#define FLASH FFat
#define F_NAME "FATFS"
#define buf_size 50

#define SYS_EN 35

static const uint32_t screenWidth = WIDTH;
static const uint32_t screenHeight = HEIGHT;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * buf_size];

#define SCREEN_TOUCH_TIMEOUT 10000
#define SCREEN_TOUCH_TIMEOUT_FADE_MS 620

static uint8_t lcd_brightness = 75;
static unsigned long time_of_last_touch = 0;
bool ui_ready = false;

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Touch_CST816S _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      // SPIバスの設定
      cfg.spi_host = SPI; // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // ※ ESP-IDFバージョンアップに伴い、VSPI_HOST , HSPI_HOSTの記述は非推奨になるため、エラーが出る場合は代わりにSPI2_HOST , SPI3_HOSTを使用してください。
      cfg.spi_mode = 0;                  // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 80000000;         // 传输时的SPI时钟（最高80MHz，四舍五入为80MHz除以整数得到的值）
      cfg.freq_read = 20000000;          // 接收时的SPI时钟
      cfg.spi_3wire = true;              // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock = true;               // 使用事务锁时设置为 true
      cfg.dma_channel = SPI_DMA_CH_AUTO; // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
      // ※ ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
      cfg.pin_sclk = SCLK; // SPIのSCLKピン番号を設定
      cfg.pin_mosi = MOSI; // SPIのCLKピン番号を設定
      cfg.pin_miso = MISO; // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc = DC;     // SPIのD/Cピン番号を設定  (-1 = disable)

      _bus_instance.config(cfg);              // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
    }

    {                                      // 表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

      cfg.pin_cs = CS;   // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst = RST; // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy = -1; // BUSYが接続されているピン番号 (-1 = disable)

      // ※ 以下の設定値はパネル毎に一般的な初期値が設定さ BUSYが接続されているピン番号 (-1 = disable)れていますので、不明な項目はコメントアウトして試してみてください。

      cfg.memory_width = WIDTH;   // ドライバICがサポートしている最大の幅
      cfg.memory_height = HEIGHT; // ドライバICがサポートしている最大の高さ
      cfg.panel_width = WIDTH;    // 実際に表示可能な幅
      cfg.panel_height = HEIGHT;  // 実際に表示可能な高さ
      cfg.offset_x = OFFSET_X;    // パネルのX方向オフセット量
      cfg.offset_y = OFFSET_Y;    // パネルのY方向オフセット量
      cfg.offset_rotation = 0;    // 值在旋转方向的偏移0~7（4~7是倒置的）
      cfg.dummy_read_pixel = 8;   // 在读取像素之前读取的虚拟位数
      cfg.dummy_read_bits = 1;    // 读取像素以外的数据之前的虚拟读取位数
      cfg.readable = false;       // 如果可以读取数据，则设置为 true
      cfg.invert = true;          // 如果面板的明暗反转，则设置为 true
      cfg.rgb_order = RGB_ORDER;  // 如果面板的红色和蓝色被交换，则设置为 true
      cfg.dlen_16bit = false;     // 对于以 16 位单位发送数据长度的面板，设置为 true
      cfg.bus_shared = false;     // 如果总线与 SD 卡共享，则设置为 true（使用 drawJpgFile 等执行总线控制）

      _panel_instance.config(cfg);
    }

    {                                      // Set backlight control. (delete if not necessary)
      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = BL;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 1; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    { // タッチスクリーン制御の設定を行います。（必要なければ削除）
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;        // タッチスクリーンから得られる最小のX値(生の値)
      cfg.x_max = WIDTH;    // タッチスクリーンから得られる最大のX値(生の値)
      cfg.y_min = 0;        // タッチスクリーンから得られる最小のY値(生の値)
      cfg.y_max = HEIGHT;   // タッチスクリーンから得られる最大のY値(生の値)
      cfg.pin_int = TP_INT; // INTが接続されているピン番号
      // cfg.pin_rst = TP_RST;
      cfg.bus_shared = false;  // 画面と共通のバスを使用している場合 trueを設定
      cfg.offset_rotation = 0; // 表示とタッチの向きのが一致しない場合の調整 0~7の値で設定
      cfg.i2c_port = 0;        // 使用するI2Cを選択 (0 or 1)
      cfg.i2c_addr = 0x15;     // I2Cデバイスアドレス番号
      cfg.pin_sda = I2C_SDA;   // SDAが接続されているピン番号
      cfg.pin_scl = I2C_SCL;   // SCLが接続されているピン番号
      cfg.freq = 400000;       // I2Cクロックを設定

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // タッチスクリーンをパネルにセットします。
    }

    setPanel(&_panel_instance); // 使用するパネルをセットします。
  }
};

LGFX tft;
Preferences prefs;

// Declare an instance of Qmi8658c
Qmi8658c *qmi8658c;

/* QMI8658C configuration */
qmi8658_cfg_t qmi8658_cfg = {
    .qmi8658_mode = qmi8658_mode_gyro_only, // Set the QMI8658C mode to dual mode
    .acc_scale = acc_scale_2g,              // Set the accelerometer scale to ±2g
    .acc_odr = acc_odr_3,                   // Set the accelerometer output data rate (ODR)
    .gyro_scale = gyro_scale_2048dps,       // Set the gyroscope scale to ±2048 dps
    .gyro_odr = gyro_odr_250,               // Set the gyroscope output data rate (ODR) to 8000Hz
};

qmi8658_result_t qmi8658_result;
qmi_data_t sensor_data; // Declare a variable to store sensor data

void hal_setup(void);
void hal_loop(void);
void update_gyro();
float get_gyro_x();
float get_gyro_y();
float get_gyro_z();

void flashDrive_cb(lv_event_t *e);
void driveList_cb(lv_event_t *e);

void checkLocal();

bool lvImgHeader(uint8_t *byteArray, uint8_t cf, uint16_t w, uint16_t h);

/* Display flushing */
void lcd_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

void virtual_leds_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  Timber.i("led display flush start %d,%d -> %d,%d", area->x1, area->y1, area->x2, area->y2);

  int non_black_pixel_count = 0;

  // log the buffer
  for (int i = 0; i < DISPLAY_VIRTUAL_LEDS_PIXEL_COUNT; i++)
  {
    uint16_t color = color_p[i].full;
    if (color != 0x0000)
    {
      non_black_pixel_count++;
      // if (color != 0xA310)
      // {
      //   Timber.i("%02X ", color);
      // }
    }
  }
  Timber.i("led display flush end");
  Timber.i("non_black_pixel_count: %d out of %d", non_black_pixel_count, DISPLAY_VIRTUAL_LEDS_PIXEL_COUNT);
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint8_t gesture;
  uint16_t touchX, touchY;

  touched = tft.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;
    time_of_last_touch = millis();
  }
}

String heapUsage()
{
  String usage;
  uint32_t total = ESP.getHeapSize();
  uint32_t free = ESP.getFreeHeap();
  usage += "Total: " + String(total);
  usage += ", Free: " + String(free);
  usage += "\nUsage: " + String(((total - free) * 1.0) / total * 100, 2) + "%";
  return usage;
}

void screenBrightness(uint8_t value)
{
  tft.setBrightness(value);
}

void onBrightnessChange(lv_event_t *e)
{
  // Your code here
  lv_obj_t *slider = lv_event_get_target(e);
  int v = lv_slider_get_value(slider);
  screenBrightness(v);

  prefs.putInt("brightness", v);
}

#include <deque>
#include <string>

constexpr size_t LOG_HISTORY_SIZE = 10;
std::deque<String> logHistory;

void addLogLine(const String &line)
{
  if (logHistory.size() >= LOG_HISTORY_SIZE)
  {
    logHistory.pop_back();
  }
  logHistory.push_front(line);
}

const std::deque<String> &getLogHistory()
{
  return logHistory;
}

char *logHistoryLines()
{
  static String result;
  result = "";
  for (const auto &line : logHistory)
  {
    result += line + "\n";
  }
  return (char *)result.c_str();
}

// Update logCallback to store last 20 log lines
void logCallback(Level level, unsigned long time, String message)
{
  Serial.print(message);

  addLogLine(message.c_str());

  if (ui_ready)
  {
    lv_label_set_text_fmt(ui_LabelLogs, logHistoryLines());

    if (level == Level::ERROR)
    {
      lv_label_set_text_fmt(ui_LabelError, "Error: %s", message.c_str());
    }
  }
}

void my_log_cb(const char *buf)
{
  Serial.write(buf, strlen(buf));
}

void loadSplash()
{
  int w = 240;
  int h = 280;
  int xOffset = 0;
  int yOffset = 0;
  tft.fillScreen(TFT_BLACK);
  screenBrightness(lcd_brightness);
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      tft.writePixel(x + xOffset, y + yOffset, uint16_t(splash[(((y * w) + x) * 2) + 1] << 8 | splash[(((y * w) + x) * 2)]));
    }
  }

  delay(700);
}

void setup_virtual_leds_display()
{

  lv_disp_draw_buf_init(&virtual_leds_draw_buf, virtual_leds_buf, NULL, DISPLAY_VIRTUAL_LEDS_WIDTH * DISPLAY_VIRTUAL_LEDS_HEIGHT);

  lv_disp_drv_init(&virtual_leds_drv);
  virtual_leds_drv.hor_res = DISPLAY_VIRTUAL_LEDS_WIDTH;
  virtual_leds_drv.ver_res = DISPLAY_VIRTUAL_LEDS_HEIGHT;
  virtual_leds_drv.flush_cb = virtual_leds_disp_flush;
  virtual_leds_drv.draw_buf = &virtual_leds_draw_buf;
  virtual_leds_drv.full_refresh = true;
  // virtual_leds_drv.direct_mode = true;
  display_virtual_leds = lv_disp_drv_register(&virtual_leds_drv);
}

void setup_virtual_leds_ui_screen()
{
  Timber.i("setup_virtual_leds_ui_screen");
  lv_disp_set_default(display_virtual_leds);
  virtual_leds_ui_screen = lv_obj_create(NULL);

  lv_obj_set_size(virtual_leds_ui_screen, DISPLAY_VIRTUAL_LEDS_WIDTH, DISPLAY_VIRTUAL_LEDS_HEIGHT);
  lv_obj_set_pos(virtual_leds_ui_screen, 0, 0);
  lv_obj_set_style_bg_color(virtual_leds_ui_screen, lv_color_hex(0x000000), LV_PART_MAIN);

  // add an image to the screen
  image_on_virtual_screen = lv_img_create(virtual_leds_ui_screen);
  lv_obj_set_size(image_on_virtual_screen, DISPLAY_VIRTUAL_LEDS_WIDTH, DISPLAY_VIRTUAL_LEDS_HEIGHT);
  lv_obj_set_pos(image_on_virtual_screen, 0, 0);
  lv_obj_set_style_bg_color(image_on_virtual_screen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(image_on_virtual_screen, LV_OPA_COVER, LV_PART_MAIN);
  lv_img_set_src(image_on_virtual_screen, &boomtown_bw_72_square);

  lv_scr_load_anim(virtual_leds_ui_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);

  lv_disp_set_default(display_lcd);
}

bool allImagesSelected()
{
  // check if all selected
  for (int i = 0; i < lv_obj_get_child_cnt(ui_ContainerImages); i++)
  {
    lv_obj_t *button = lv_obj_get_child(ui_ContainerImages, i);
    if (!lv_obj_has_state(button, LV_STATE_CHECKED))
    {
      return false;
    }
  }
  return true;
}

void onSelectAllImages(lv_event_t *e)
{
  bool allSelected = allImagesSelected();

  // if we get here, all images are selected so deselect all
  for (int i = 0; i < lv_obj_get_child_cnt(ui_ContainerImages); i++)
  {
    lv_obj_t *button = lv_obj_get_child(ui_ContainerImages, i);
    if (allSelected)
    {
      lv_obj_clear_state(button, LV_STATE_CHECKED);
    }
    else
    {
      lv_obj_add_state(button, LV_STATE_CHECKED);
    }
  }
}

void ui_event_ImageButton(lv_event_t *e)
{
  lv_event_code_t event_code = lv_event_get_code(e);
  lv_obj_t *target = lv_event_get_target(e);

  if (event_code == LV_EVENT_CLICKED)
  {
    setFirstSelectedImageIndex();
  }

  if (event_code == LV_EVENT_LONG_PRESSED)
  {
    onSelectAllImages(e);
  }
}

lv_obj_t *createImageButton(const lv_img_dsc_t *image_src)
{
  lv_obj_t *imageButton = lv_btn_create(ui_ContainerImages);
  lv_obj_set_width(imageButton, image_src->header.w + 8);
  lv_obj_set_height(imageButton, image_src->header.h + 8);
  lv_obj_set_align(imageButton, LV_ALIGN_CENTER);
  lv_obj_set_flex_flow(imageButton, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(imageButton, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_add_flag(imageButton, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
  lv_obj_clear_flag(imageButton, LV_OBJ_FLAG_SCROLLABLE);                            /// Flags
  lv_obj_set_style_bg_color(imageButton, lv_color_hex(0x5BCEFA), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(imageButton, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_left(imageButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_right(imageButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_top(imageButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_bottom(imageButton, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(imageButton, lv_color_hex(0xFF69B4), LV_PART_MAIN | LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(imageButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
  lv_obj_add_event_cb(imageButton, ui_event_ImageButton, LV_EVENT_ALL, NULL);

  lv_obj_t *image = lv_img_create(imageButton);
  lv_obj_set_width(image, image_src->header.w);
  lv_obj_set_height(image, image_src->header.h);
  lv_obj_set_align(image, LV_ALIGN_CENTER);
  lv_obj_set_style_radius(image, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_clip_corner(image, true, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_img_set_src(image, image_src);

  return imageButton;
}

void loadImageButtons()
{
  lv_obj_clean(ui_ContainerImages);

  // loop throw image_array
  for (auto index = 0; index < ARRAY_SIZE(image_array); index++)
  {
    lv_obj_t *button = createImageButton(image_array[index]);
  }
}

void setLEDBrightness(uint8_t brightness)
{
  led_brightness = brightness;
  led_brightness = lv_slider_get_value(ui_SliderLEDBrightness);

  lv_label_set_text_fmt(ui_LabelLEDBrightness, "LED Brightness: %d", led_brightness);
}

void hal_setup()
{

  esp_pm_config_t pm_config = {
      .max_freq_mhz = 240,
      .min_freq_mhz = 240,
      .light_sleep_enable = false,
  };
  esp_pm_configure(&pm_config);

  // System enable - CRITICAL!
  pinMode(SYS_EN, OUTPUT);
  digitalWrite(SYS_EN, HIGH);
  // wait for power to stabilise
  delay(500);

  Serial.begin(115200); /* prepare for possible serial debug */

  Timber.setLogCallback(logCallback);
  Timber.showTime(true);

  Timber.i("Starting up device");

  prefs.begin("my-app");

  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.fillScreen(TFT_BLACK);
  loadSplash();

  Timber.i(heapUsage());

  lv_init();

  Timber.i("Setup virtual leds display");
  // setup_virtual_leds_display();

  Timber.i("Setup lcd display");
  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * buf_size);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = lcd_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  display_lcd = lv_disp_drv_register(&disp_drv);
  lv_disp_set_default(display_lcd);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.disp = display_lcd;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  //  lv_log_register_print_cb(my_log_cb);

  // delay(500); // Delay for sensor initialization
  qmi8658c = new Qmi8658c(QMI_ADDRESS, QMI8658C_I2C_FREQUENCY);
  qmi8658_result = qmi8658c->open(&qmi8658_cfg);
  // Delay for sensor initialization

  //_lv_fs_init();

  ui_init();
  // setup_virtual_leds_ui_screen();

  Timber.i(heapUsage());

  // bool fsState = setupFS();
  // if (fsState)
  // {
  //   // driveList_cb(NULL);
  //   Serial.println("Setup FS success");
  // }
  // else
  // {
  //   Serial.println("Setup FS failed");
  //   // showError(F_NAME, "Failed to mount the partition");
  // }

  // String custom = prefs.getString("custom", "");

  lv_disp_load_scr(ui_Screen1);

  ui_ready = true;

  Timber.i("IMU State: %s", qmi8658c->resultToString(qmi8658_result));

  lv_dropdown_clear_options(ui_DropdownMode);
  for (int i = 0; i < ARRAY_SIZE(gPatternNames); i++)
  {
    lv_dropdown_add_option(ui_DropdownMode, gPatternNames[i], i);
    // Timber.i("Pattern %d: %s", i, gPatternNames[i]);
  }

  lv_dropdown_clear_options(ui_DropdownFirePalette);
  for (int i = 0; i < ARRAY_SIZE(palette_names); i++)
  {
    lv_dropdown_add_option(ui_DropdownFirePalette, palette_names[i], i);
    // Timber.i("Palette %d: %s", i, palette_names[i]);
  }

  selected_hsv = lv_colorwheel_get_hsv(ui_ColorWheel);

  setModeContainer(0);
  loadImageButtons();
  setLEDBrightness(led_brightness);

  Timber.i("Setup done");
}

void update_gyro()
{
  qmi8658c->read(&sensor_data);
}

qmi_data_t get_sensor_data()
{
  return sensor_data;
}

float get_gyro_x()
{
  return sensor_data.gyro_xyz.x;
}

float get_gyro_y()
{
  return sensor_data.gyro_xyz.y;
}

float get_gyro_z()
{
  return sensor_data.gyro_xyz.z;
}

float get_acc_x()
{
  return sensor_data.acc_xyz.x;
}

float get_acc_y()
{
  return sensor_data.acc_xyz.y;
}

float get_acc_z()
{
  return sensor_data.acc_xyz.z;
}

float get_temp()
{
  return sensor_data.temperature;
}

void hal_loop()
{

  lv_timer_handler(); /* let the GUI do its work */

  FastLED.setBrightness(led_brightness);

  unsigned long current_time = millis();
  unsigned long timeout_time = time_of_last_touch + SCREEN_TOUCH_TIMEOUT;
  long time_remaining = timeout_time - current_time;
  if (time_remaining < 0)
  {
    // If time_remaining is between 0 and -SCREEN_TOUCH_TIMEOUT_FADE_MS, interpolate brightness from 200 to 0
    if (time_remaining >= -SCREEN_TOUCH_TIMEOUT_FADE_MS)
    {
      int brightness = (int)(((double)lcd_brightness) * (time_remaining + SCREEN_TOUCH_TIMEOUT_FADE_MS) / (double)SCREEN_TOUCH_TIMEOUT_FADE_MS);
      if (brightness < 0)
        brightness = 0;
      if (brightness > lcd_brightness)
        brightness = lcd_brightness;
      screenBrightness(brightness);
    }
    else
      screenBrightness(0);
  }
  else
  {
    screenBrightness(lcd_brightness);
  }

  // lv_disp_t *display = lv_disp_get_default();
  // lv_obj_t *actScr = lv_disp_get_scr_act(display);
}

void leds_setup()
{
  // Serial.begin(115200);
  delay(1000);
  Timber.i("Initializing LEDs");
  setup_leds();

  leds_setup_completed = true;
  setCurrentPatternLabel();
}

void leds_loop()
{
  // do some periodic updates
  EVERY_N_MILLISECONDS_DYNAMIC(hue_delay) { gHue--; } // slowly cycle the "base color" through the rainbow

  EVERY_N_MILLISECONDS(75)
  {
    if (gCurrentPatternNumber == 0 || always_swipe)
    {
      update_gyro();
      gyro_z = get_gyro_z();
      lv_label_set_text_fmt(ui_LabelGyro, "Gyro Z: %.2f", gyro_z);
    }
  }

  // are we in mask mode?
  if (gCurrentPatternNumber > 0 && always_swipe)
  {
    detectSwipe();
    return;
  }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  if (gCurrentPatternNumber > 0)
  {
    addGlitter(glitter);
    mirror_leds();
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  FastLED.countFPS();

  // EVERY_N_SECONDS(5) { nextPattern(); } // change patterns periodically
  EVERY_N_SECONDS(2)
  {
    // Timber.i("LED FPS: %d", FastLED.getFPS());
    lv_label_set_text_fmt(ui_LabelTemp, "Temperature: %.2fC", get_temp());
    lv_label_set_text_fmt(ui_LabelFPS, "LED FPS: %d", FastLED.getFPS());
    lv_label_set_text(ui_LabelMemory, heapUsage().c_str());
  }
}

void onButtonNextPatternClicked(lv_event_t *e)
{
  // setup_virtual_leds_ui_screen();
  nextPattern();
}

void onButtonPrevClicked(lv_event_t *e)
{
  // setup_virtual_leds_ui_screen();
  prevPattern();
}

void onFireSparkingChanged(lv_event_t *e)
{
  fire_sparking = lv_slider_get_value(ui_SliderFireSparking);
}

void onFireCoolingChanged(lv_event_t *e)
{
  fire_cooling = lv_slider_get_value(ui_SliderFireCooling);
}

void onDropdownPatternChanged(lv_event_t *e)
{
  int selectedIndex = lv_dropdown_get_selected(ui_DropdownMode);
  Timber.i("onDropdownPatternChanged %d", selectedIndex);
  setPatternIndex(selectedIndex);
}

void onLEDBrightnessChanged(lv_event_t *e)
{
  setLEDBrightness(led_brightness);
}

void onFlashModeStart(lv_event_t *e)
{
  setLEDBrightness(0);
}

void onFlashModeEnd(lv_event_t *e)
{
  led_brightness = lv_slider_get_value(ui_SliderLEDBrightness);
}

void onFlashPressed(lv_event_t *e)
{
  led_brightness = lv_slider_get_value(ui_SliderLEDBrightness);
}

void onFlashReleased(lv_event_t *e)
{
  led_brightness = 0;
}

void onHueDelayChanged(lv_event_t *e)
{
  hue_delay = lv_slider_get_value(ui_SliderHueDelay);
}

void onGlitterChanged(lv_event_t *e)
{
  glitter = lv_slider_get_value(ui_SliderGlitter);
}

void onAlwaysSwipeChecked(lv_event_t *e)
{
  always_swipe = true;
}

void onAlwaysSwipeUnchecked(lv_event_t *e)
{
  always_swipe = false;
}

void onColourWheelChanged(lv_event_t *e)
{
  selected_hsv = lv_colorwheel_get_hsv(ui_ColorWheel);
  //  Timber.i("Colour wheel changed: hue: %d, saturation: %d, value: %d", selected_hsv.h, selected_hsv.s, selected_hsv.v);
}

void onDropdownFirePaletteChanged(lv_event_t *e)
{
  selected_palette_index = lv_dropdown_get_selected(ui_DropdownFirePalette);
}

void onSwipesPerImageChanged(lv_event_t *e)
{
  swipes_per_image = lv_slider_get_value(ui_SliderSwipesPerImage);
  lv_label_set_text_fmt(ui_LabelSwipesPerImage, "%d %s Per Image", swipes_per_image, swipes_per_image == 1 ? "Swipe" : "Swipes");
}

void onRandomAfterSwipeChecked(lv_event_t *e)
{
  randomise_after_swipe = true;
}

void onRandomAfterSwipeUnchecked(lv_event_t *e)
{
  randomise_after_swipe = false;
}

void onRandomColourAfterSwipeChecked(lv_event_t *e)
{
  randomise_colour_after_swipe = true;
}

void onRandomColourAfterSwipeUnchecked(lv_event_t *e)
{
  randomise_colour_after_swipe = false;
}
