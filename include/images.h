#include "images/boomtown_bw_72_square.c"
#include "images/mario.c"
#include "images/bt_text.c"
#include "images/boomtown_new.c"
#include "images/heart1.c"
#include "images/heart2.c"
#include "images/palestine_heart.c"
#include "images/hearts_wide.c"
#include "images/hearts_superwide.c"

#include "images/eachother.c"
#include "images/yourself.c"
#include "images/respect_respect.c"
#include "images/theplanet.c"

#include "images/happy.c"
#include "images/smiley.c"
#include "images/vegan.c"
#include "images/shroom.c"

#include "images/music_note.c"
#include "images/mushroom_1.c"

#include "images/heart_neon.c"
#include "images/star_cute.c"
#include "images/fairy_dust.c"
#include "images/speaker.c"
#include "images/dog_1.c"
#include "images/dog_2.c"
#include "images/dog_3.c"
#include "images/dog_4.c"
#include "images/dance_man_0.c"
#include "images/dance_man_1.c"
#include "images/dance_man_2.c"
#include "images/boom.c"
#include "images/bam.c"
#include "images/pow.c"
#include "images/lqbtq.c"
#include "images/wave_1.c"
#include "images/wave_2.c"
#include "images/wave_3.c"
#include "images/dickbutt.c"

// Map of image names to their descriptor objects

#include <map>
#include <string>

// Extern image descriptors
extern const lv_img_dsc_t boomtown_new;
extern const lv_img_dsc_t boomtown_bw_72_square;
extern const lv_img_dsc_t bt_text;
extern const lv_img_dsc_t lqbtq;
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
extern const lv_img_dsc_t hearts_superwide;
extern const lv_img_dsc_t mario;
extern const lv_img_dsc_t eachother;
extern const lv_img_dsc_t yourself;
extern const lv_img_dsc_t respect_respect;
extern const lv_img_dsc_t theplanet;

extern const lv_img_dsc_t heart_neon;
extern const lv_img_dsc_t star_cute;
extern const lv_img_dsc_t fairy_dust;
extern const lv_img_dsc_t speaker;
extern const lv_img_dsc_t dog_1;
extern const lv_img_dsc_t dog_2;
extern const lv_img_dsc_t dog_3;
extern const lv_img_dsc_t dog_4;
extern const lv_img_dsc_t dance_man_0;
extern const lv_img_dsc_t dance_man_1;
extern const lv_img_dsc_t dance_man_2;
extern const lv_img_dsc_t boom;
extern const lv_img_dsc_t bam;
extern const lv_img_dsc_t pow_1;

extern const lv_img_dsc_t wave_1;
extern const lv_img_dsc_t wave_2;
extern const lv_img_dsc_t wave_3;
extern const lv_img_dsc_t dickbutt;

static const lv_img_dsc_t *image_array[] = {
    &boomtown_new,
    &boomtown_bw_72_square,
    &bt_text,
    &lqbtq,
    &heart1,
    &heart2,
    &palestine_heart,
    &heart_neon,
    &hearts_wide,
    &hearts_superwide,
    &star_cute,
    &fairy_dust,
    &mario,
    &mushroom_1,

    &respect_respect,
    &yourself,
    &respect_respect,
    &eachother,
    &respect_respect,
    &theplanet,

    &happy,
    &smiley,
    &vegan,
    &shroom,
    &music_note,
    &speaker,
    &dog_1,
    &dog_2,
    &dog_3,
    &dog_4,
    &dance_man_0,
    &dance_man_1,
    &dance_man_2,
    &boom,
    &bam,
    &pow_1,
    &wave_1,
    &wave_2,
    &wave_3,
    &dickbutt,
};