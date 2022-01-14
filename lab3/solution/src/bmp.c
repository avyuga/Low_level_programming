#include "../include/bmp.h"

#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct __attribute__((packed)) bmp_header {
    uint16_t bfType;
    uint32_t  bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;    // 24 r g b
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};


static bool bmp_header_valid(struct bmp_header const* bh){
    return bh->biBitCount == 24;
}

static size_t bmp_padding(size_t width){
    return width % 4;
}

struct result_image result_image_error(enum result_image_status s){
    return (struct result_image){.status = s};
}

struct result_image image_from_bmp(FILE* f){
    struct bmp_header bh = { 0 };
    if (!fread(&bh, sizeof(struct bmp_header), 1, f)){
        return result_image_error(BMF_FILE_CORRUPTED);
    }
    if (!bmp_header_valid(&bh)) return result_image_error(BMF_INVALID_FORMAT);
    struct pixel* pixels = malloc(sizeof(struct pixel) * bh.biHeight * bh.biWidth);
    for (uint32_t y = 0; y < bh.biHeight; y++) {
        if (!fread(pixels + y * bh.biWidth, bh.biWidth * sizeof(struct pixel), 1, f)) {
            free(pixels);
            return result_image_error(BMF_FILE_CORRUPTED);
        }
        if (fseek(f, bmp_padding(bh.biWidth), SEEK_CUR)) {
            free(pixels);
            return result_image_error(BMF_FILE_CORRUPTED);
        }
    }
    return (struct result_image){.status = BMF_OK,
            .img = (struct image){.height = bh.biHeight, .width = bh.biWidth, .pixels = pixels} };
}

static size_t image_bmp_image_size(struct image const* img){
    return img->height * img->width * sizeof(struct pixel) + img->height * bmp_padding(img->width);
}
static size_t image_bmp_file_size(struct image const* img){
    return image_bmp_image_size(img) + sizeof(struct bmp_header);
}

static struct bmp_header bmp_header_fill(struct image const* img){
    return (struct bmp_header){
            .bfType = 19778,
            .bfileSize = image_bmp_file_size(img),
            .bOffBits = sizeof(struct bmp_header),
            .biSize = 40,
            .biWidth = img->width,
            .biHeight = img->height,
            .biPlanes = 1,
            .biBitCount = 24,
            .biSizeImage = image_bmp_image_size(img)
    };
}

bool image_to_bmp(struct image const* img, FILE* f) {
    struct bmp_header bh = bmp_header_fill(img);
    if (!fwrite(&bh, sizeof(struct bmp_header), 1, f)) {
        return false;
    }
    for (uint32_t y = 0; y < img->height; y++) {
        if (!fwrite(img->pixels + y * img->width, img->width * sizeof(struct pixel), 1, f)) {
            return false;
        }
        {
            char padding[4] = {0};
            if (!fwrite(padding, bmp_padding(img->width), 1, f)) return false;
        }
    }
    return true;
}
