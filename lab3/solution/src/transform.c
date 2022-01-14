#include "image.h"
#include "transform.h"
#include <malloc.h>

struct image rotate(struct image const img){
    size_t width = img.width;
    size_t height = img.height;
    struct image result = {.width = height, .height = width};
    result.pixels = malloc(height * width * sizeof(struct pixel));
    for (size_t i = 0; i < width*height; i++){
        size_t x = i / width;
        size_t y = i % width;
        result.pixels[(height - x - 1) + y * height] = img.pixels[i];
    }

    return result;
}
