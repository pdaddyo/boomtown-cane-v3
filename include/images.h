#include "images/boomtown_bw_72_square.c"
#include "images/boomtown_with_text.c"
#include "images/mario.c"

// Map of image names to their descriptor objects

#include <map>
#include <string>

// Extern image descriptors
extern const lv_img_dsc_t boomtown_bw_72_square;
extern const lv_img_dsc_t boomtown_with_text;
extern const lv_img_dsc_t mario;

// Map of image names to descriptors
static const std::map<std::string, const lv_img_dsc_t *> image_map = {
    {"mario", &mario},
    {"boomtown_bw_72_square", &boomtown_bw_72_square},
    {"boomtown_with_text", &boomtown_with_text},
};

static const lv_img_dsc_t *image_array[] = {
    &mario,
    &boomtown_bw_72_square,
    &boomtown_with_text,
};