#include "image.h"
#include "transform.h"
#include <malloc.h>

struct pixel get_pixel(struct image const init, size_t x, size_t y) {
    return init.pixels[x * init.width + y];
}

void set_pixel(struct image *img, struct pixel const px, size_t x, size_t y) {
    img->pixels[x * img->width + y] = px;
}

struct image rotate(struct image const img) {
    size_t width = img.width;
    size_t height = img.height;
    struct image result = {.width = height, .height = width};
    result.pixels = malloc(height * width * sizeof(struct pixel));
    for (size_t i = 0; i < width * height; i++) {
        size_t x = get_x(img, i);
        size_t y = get_y(img, i);
        set_pixel(&result, get_pixel(img, x, y), y, height - x - 1);
    }

    return result;
}
