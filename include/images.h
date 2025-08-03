#include "images/boomtown_bw_72_square.c"

// Map of image names to their descriptor objects

#include <map>
#include <string>

// Extern image descriptors
extern const lv_img_dsc_t boomtown_bw_72_square;

// Map of image names to descriptors
static const std::map<std::string, const lv_img_dsc_t *> image_map = {
    {"boomtown_bw_72_square", &boomtown_bw_72_square}};
