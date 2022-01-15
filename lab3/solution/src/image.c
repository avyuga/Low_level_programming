#include "image.h"

size_t get_x(struct image const img, size_t i) {
    return i / img.width;
}

size_t get_y(struct image const img, size_t i) {
    return i % img.width;
}
