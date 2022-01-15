#include "../include/bmp.h"

#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BF_TYPE 19778 // сигнатура BMP-формата
#define BI_PLANES 1 // специальное значение
#define BI_BIT_COUNT 24 // глубина цвета
#define BI_SIZE 40 // размер структуры, указ. также на её версию


struct __attribute__((packed)) bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
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


static inline bool bmp_header_valid(struct bmp_header const *bh) {
    return bh->biBitCount == BI_BIT_COUNT;
}

static inline size_t bmp_padding(size_t width) {
    return width % 4;
}

struct result_image result_image_error(enum result_image_status s) {
    return (struct result_image) {.status = s};
}

struct result_image image_from_bmp(FILE *f) {
    struct bmp_header bh = {0};
    int64_t header_read = fread(&bh, sizeof(struct bmp_header), 1, f);
    if (!header_read) {
        return result_image_error(BMF_FILE_CORRUPTED);
    }
    bool header_valid = bmp_header_valid(&bh);
    if (!header_valid) return result_image_error(BMF_INVALID_FORMAT);

    struct pixel *pixels = malloc(sizeof(struct pixel) * bh.biHeight * bh.biWidth);
    for (uint32_t y = 0; y < bh.biHeight; y++) {
        int64_t pixels_read = fread(pixels + y * bh.biWidth, bh.biWidth * sizeof(struct pixel), 1, f);
        if (!pixels_read) {
            free(pixels);
            return result_image_error(BMF_FILE_CORRUPTED);
        }
        size_t padding = bmp_padding(bh.biWidth);
        int64_t bytes_seek = 0;
        if (padding) bytes_seek = fseek(f, padding, SEEK_CUR);
        if (bytes_seek) {
            free(pixels);
            return result_image_error(BMF_FILE_CORRUPTED);
        }
    }
    return (struct result_image) {.status = BMF_OK,
            .img = (struct image) {.height = bh.biHeight, .width = bh.biWidth, .pixels = pixels}};
}

static size_t image_bmp_image_size(struct image const *img) {
    return img->height * img->width * sizeof(struct pixel) + img->height * bmp_padding(img->width);
}

static size_t image_bmp_file_size(struct image const *img) {
    return image_bmp_image_size(img) + sizeof(struct bmp_header);
}

static struct bmp_header bmp_header_fill(struct image const *img) {
    return (struct bmp_header) {
            .bfType = BF_TYPE,
            .bfileSize = image_bmp_file_size(img),
            .bOffBits = sizeof(struct bmp_header),
            .biSize = BI_SIZE,
            .biWidth = img->width,
            .biHeight = img->height,
            .biPlanes = BI_PLANES,
            .biBitCount = BI_BIT_COUNT,
            .biSizeImage = image_bmp_image_size(img)
    };
}

bool image_to_bmp(struct image const *img, FILE *f) {
    struct bmp_header bh = bmp_header_fill(img);
    int64_t header_write = fwrite(&bh, sizeof(struct bmp_header), 1, f);
    if (!header_write) {
        return false;
    }
    for (uint32_t y = 0; y < img->height; y++) {
        int64_t pixels_write = fwrite(img->pixels + y * img->width, img->width * sizeof(struct pixel), 1, f);
        if (!pixels_write) {
            return false;
        }
        {
            static const char padding[4] = {0};
            size_t num_padding = bmp_padding(img->width);
            int64_t padding_write = 0;
            if (num_padding) padding_write = fwrite(padding, num_padding, 1, f);
            if (!padding_write) return false;
        }
    }
    return true;
}
