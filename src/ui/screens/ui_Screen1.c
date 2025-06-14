// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.2
// LVGL version: 8.3.11
// Project name: Cane-V3-UI

#include "../ui.h"

lv_obj_t *ui_Screen1;
lv_obj_t *ui_TabView1;
lv_obj_t *ui_TabStatus;
lv_obj_t *ui_Label2;
lv_obj_t *ui_ButtonNextPattern;
lv_obj_t *ui_Label3;
lv_obj_t *ui_TabSettings;
lv_obj_t *ui_Label1;
lv_obj_t *ui_SliderLEDBrightness;

// event funtions
void ui_event_ButtonNextPattern( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_CLICKED) {
      onButtonNextPatternClicked( e );
}
}

void ui_event_SliderLEDBrightness( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
      onLEDBrightnessChanged( e );
}
}

// build funtions

void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);

ui_TabView1 = lv_tabview_create(ui_Screen1, LV_DIR_TOP, 50);
lv_obj_set_width( ui_TabView1, 240);
lv_obj_set_height( ui_TabView1, 280);
lv_obj_set_x( ui_TabView1, 1 );
lv_obj_set_y( ui_TabView1, 2 );
lv_obj_set_align( ui_TabView1, LV_ALIGN_CENTER );
lv_obj_clear_flag( ui_TabView1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_TabStatus = lv_tabview_add_tab(ui_TabView1, "Status");
lv_obj_set_flex_flow(ui_TabStatus,LV_FLEX_FLOW_ROW);
lv_obj_set_flex_align(ui_TabStatus, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

ui_Label2 = lv_label_create(ui_TabStatus);
lv_obj_set_height( ui_Label2, 55);
lv_obj_set_width( ui_Label2, lv_pct(100));
lv_obj_set_align( ui_Label2, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label2,"CaneOS\n");
lv_obj_set_style_text_align(ui_Label2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_Label2, &lv_font_montserrat_44, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_ButtonNextPattern = lv_btn_create(ui_TabStatus);
lv_obj_set_height( ui_ButtonNextPattern, 50);
lv_obj_set_width( ui_ButtonNextPattern, lv_pct(100));
lv_obj_set_x( ui_ButtonNextPattern, -97 );
lv_obj_set_y( ui_ButtonNextPattern, 43 );
lv_obj_set_align( ui_ButtonNextPattern, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_ButtonNextPattern, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK | LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_ButtonNextPattern, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_Label3 = lv_label_create(ui_ButtonNextPattern);
lv_obj_set_width( ui_Label3, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label3, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Label3, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label3,"Next Pattern");

ui_TabSettings = lv_tabview_add_tab(ui_TabView1, "Settings");

ui_Label1 = lv_label_create(ui_TabSettings);
lv_obj_set_width( ui_Label1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Label1, -41 );
lv_obj_set_y( ui_Label1, -78 );
lv_obj_set_align( ui_Label1, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label1,"LED Brightness\n");
lv_obj_set_style_text_font(ui_Label1, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_SliderLEDBrightness = lv_slider_create(ui_TabSettings);
lv_slider_set_value( ui_SliderLEDBrightness, 20, LV_ANIM_OFF);
if (lv_slider_get_mode(ui_SliderLEDBrightness)==LV_SLIDER_MODE_RANGE ) lv_slider_set_left_value( ui_SliderLEDBrightness, 0, LV_ANIM_OFF);
lv_obj_set_height( ui_SliderLEDBrightness, 30);
lv_obj_set_width( ui_SliderLEDBrightness, lv_pct(90));
lv_obj_set_x( ui_SliderLEDBrightness, -6 );
lv_obj_set_y( ui_SliderLEDBrightness, -47 );
lv_obj_set_align( ui_SliderLEDBrightness, LV_ALIGN_CENTER );
lv_obj_set_flex_flow(ui_SliderLEDBrightness,LV_FLEX_FLOW_ROW);
lv_obj_set_flex_align(ui_SliderLEDBrightness, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

lv_obj_set_style_border_color(ui_SliderLEDBrightness, lv_color_hex(0x000000), LV_PART_INDICATOR | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_SliderLEDBrightness, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
lv_obj_set_style_border_width(ui_SliderLEDBrightness, 4, LV_PART_INDICATOR| LV_STATE_DEFAULT);

lv_obj_set_style_outline_color(ui_SliderLEDBrightness, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_outline_opa(ui_SliderLEDBrightness, 255, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_outline_width(ui_SliderLEDBrightness, 2, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_outline_pad(ui_SliderLEDBrightness, 0, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_color(ui_SliderLEDBrightness, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT );
lv_obj_set_style_shadow_opa(ui_SliderLEDBrightness, 100, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_width(ui_SliderLEDBrightness, 2, LV_PART_KNOB| LV_STATE_DEFAULT);
lv_obj_set_style_shadow_spread(ui_SliderLEDBrightness, 2, LV_PART_KNOB| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_ButtonNextPattern, ui_event_ButtonNextPattern, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_SliderLEDBrightness, ui_event_SliderLEDBrightness, LV_EVENT_ALL, NULL);

}

void ui_Screen1_screen_destroy(void)
{
   if (ui_Screen1) lv_obj_del(ui_Screen1);

// NULL screen variables
ui_Screen1= NULL;
ui_TabView1= NULL;
ui_TabStatus= NULL;
ui_Label2= NULL;
ui_ButtonNextPattern= NULL;
ui_Label3= NULL;
ui_TabSettings= NULL;
ui_Label1= NULL;
ui_SliderLEDBrightness= NULL;

}
