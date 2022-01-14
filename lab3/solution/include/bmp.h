#ifndef LAB3_BMP_H
#define LAB3_BMP_H

#include "image.h"

#include <stdbool.h>
#include <stdio.h>

struct result_image{
    enum result_image_status{
        BMF_OK,
        BMF_WRONG_BITS_PER_PIXEL,
        BMF_FILE_CORRUPTED,
        BMF_INVALID_FORMAT
    } status;
    struct image img;
};

struct result_image image_from_bmp(FILE* f);
bool image_to_bmp(struct image const* img, FILE* f);
#endif //LAB3_BMP_H
