#include "images/boomtown_bw_72_square.c"
#include "images/mario.c"
#include "images/bt_text.c"
#include "images/boomtown_new.c"
#include "images/heart1.c"
#include "images/heart2.c"
#include "images/palestine_heart.c"
#include "images/hearts_wide.c"

#include "images/respect_1.c"
#include "images/respect_2.c"
#include "images/respect_3.c"

#include "images/happy.c"
#include "images/smiley.c"
#include "images/vegan.c"
#include "images/shroom.c"

#include "images/music_note.c"
#include "images/mushroom_1.c"

// Map of image names to their descriptor objects

#include <map>
#include <string>

// Extern image descriptors
extern const lv_img_dsc_t boomtown_new;
extern const lv_img_dsc_t boomtown_bw_72_square;
extern const lv_img_dsc_t bt_text;
extern const lv_img_dsc_t heart1;
extern const lv_img_dsc_t heart2;
extern const lv_img_dsc_t happy;
extern const lv_img_dsc_t smiley;
extern const lv_img_dsc_t vegan;
extern const lv_img_dsc_t shroom;
extern const lv_img_dsc_t palestine_heart;
extern const lv_img_dsc_t music_note;
extern const lv_img_dsc_t mushroom_1;
extern const lv_img_dsc_t hearts_wide;
extern const lv_img_dsc_t mario;
extern const lv_img_dsc_t respect_1;
extern const lv_img_dsc_t respect_2;
extern const lv_img_dsc_t respect_3;

// Map of image names to descriptors
static const std::map<std::string, const lv_img_dsc_t *> image_map = {
    {"boomtown_new", &boomtown_new},
    {"boomtown_bw_72_square", &boomtown_bw_72_square},
    {"bt_text", &bt_text},
    {"heart1", &heart1},
    {"heart2", &heart2},
    {"mario", &mario},
    {"respect_1", &respect_1},
    {"respect_2", &respect_2},
    {"respect_3", &respect_3},
    {"happy", &happy},
    {"smiley", &smiley},
    {"vegan", &vegan},
    {"shroom", &shroom},
    {"palestine_heart", &palestine_heart},
    {"music_note", &music_note},
    {"mushroom_1", &mushroom_1},
    {"hearts_wide", &hearts_wide},
};
static const lv_img_dsc_t *image_array[] = {
    &boomtown_new,
    &boomtown_bw_72_square,
    &bt_text,
    &heart1,
    &heart2,
    &palestine_heart,
    &hearts_wide,
    &mario,
    &mushroom_1,
    &respect_1,
    &respect_2,
    &respect_3,
    &happy,
    &smiley,
    &vegan,
    &shroom,
    &music_note,
};