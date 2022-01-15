#include <malloc.h>
#include <stdio.h>

#include "bmp.h"
#include "transform.h"

#define CODE_OK 0
#define CODE_ERROR 1

enum RETURN_MSG{
    WRONG_INPUT,
    READ_FIRST_ERROR,
    READ_SECOND_ERROR,
    TRANSFORM_ERROR,
    WRITE_ERROR,
    OK
};

int print_return_message(enum RETURN_MSG msg) {
    switch (msg){
        case WRONG_INPUT:
            puts("Wrong number of input arguments");
            return CODE_ERROR;
        case READ_FIRST_ERROR:
            puts("Error while reading the first file");
            return CODE_ERROR;
        case READ_SECOND_ERROR:
            puts("Error while reading the second file");
            return CODE_ERROR;
        case TRANSFORM_ERROR:
            puts("Error while processing file");
            return CODE_ERROR;
        case WRITE_ERROR:
            puts("Error while writing to file");
            return CODE_ERROR;
        case OK:
        default:
            return CODE_OK;
    }
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv; // suppress 'unused parameters' warning
    enum RETURN_MSG msg = OK;
    if (argc != 3) {
        msg = WRONG_INPUT;
        goto out;
    }
    char *file_in = argv[1];
    char *file_out = argv[2];
    FILE *f1 = fopen(file_in, "rb");
    if (!f1) {
        msg = READ_FIRST_ERROR;
        goto out;
    }
    FILE *f2 = fopen(file_out, "wb");
    if (!f2) {
        msg = READ_SECOND_ERROR;
        goto close_first_file;
    }

    struct result_image r = image_from_bmp(f1);
    if (r.status != BMF_OK) {
        goto free_image_pixels_and_close_second_file;
    }
    struct image rotated = rotate(r.img);
    if (!image_to_bmp(&rotated, f2)) {
        msg = WRITE_ERROR;
        goto free_rotated_pixels;
    }

    free_rotated_pixels:
        free(rotated.pixels);
    free_image_pixels_and_close_second_file:
        free(r.img.pixels);
        fclose(f2);
    close_first_file:
        fclose(f1);
    out:
        return print_return_message(msg);

}
