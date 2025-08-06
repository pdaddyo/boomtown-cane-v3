#include "images/boomtown_bw_72_square.c"
#include "images/boomtown_with_text.c"
#include "images/mario.c"
#include "images/bt_text.c"
#include "images/boomtown_new.c"
#include "images/heart1.c"
#include "images/heart2.c"
#include "images/respect_1.c"
#include "images/respect_2.c"
#include "images/respect_3.c"

// Map of image names to their descriptor objects

#include <map>
#include <string>

// Extern image descriptors
extern const lv_img_dsc_t boomtown_new;
extern const lv_img_dsc_t boomtown_bw_72_square;
extern const lv_img_dsc_t boomtown_with_text;
extern const lv_img_dsc_t bt_text;
extern const lv_img_dsc_t heart1;
extern const lv_img_dsc_t heart2;
extern const lv_img_dsc_t mario;
extern const lv_img_dsc_t respect_1;
extern const lv_img_dsc_t respect_2;
extern const lv_img_dsc_t respect_3;

// Map of image names to descriptors
static const std::map<std::string, const lv_img_dsc_t *> image_map = {
    {"boomtown_new", &boomtown_new},
    {"boomtown_bw_72_square", &boomtown_bw_72_square},
    {"boomtown_with_text", &boomtown_with_text},
    {"bt_text", &bt_text},
    {"heart1", &heart1},
    {"heart2", &heart2},
    {"mario", &mario},
    {"respect_1", &respect_1},
    {"respect_2", &respect_2},
    {"respect_3", &respect_3},
};

static const lv_img_dsc_t *image_array[] = {
    &boomtown_new,
    &boomtown_bw_72_square,
    &boomtown_with_text,
    &bt_text,
    &heart1,
    &heart2,
    &mario,
    &respect_1,
    &respect_2,
    &respect_3,
};