#ifndef LAB3_IMAGE_H
#define LAB3_IMAGE_H

#include <stddef.h>
#include <stdint.h>

struct __attribute__((packed)) pixel { uint8_t b, g, r; };

struct image {
    size_t width, height;
    struct pixel* pixels;
};

size_t get_x(struct image const img, size_t i);

size_t get_y(struct image const img, size_t i);

#endif //LAB3_IMAGE_H
