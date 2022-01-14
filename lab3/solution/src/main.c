#include <malloc.h>
#include <stdio.h>

#include "bmp.h"
#include "transform.h"

void close(FILE* f1, FILE* f2){
    fclose(f1);
    fclose(f2);
}

int main( int argc, char** argv ) {
    (void) argc;
    (void) argv; // suppress 'unused parameters' warning
    if (argc != 3){
        printf("%s", "Wrong number of input arguments\n");
        return 1;
    }
    char* file_in = argv[1];
    char* file_out = argv[2];
    FILE* f1 = fopen(file_in, "rb");
    if (!f1){
        printf("%s", "Error while reading the first file\n");
        return 1;
    }
    FILE* f2 = fopen(file_out, "wb");
    if (!f2){
        printf("%s", "Error while reading the second file\n");
        fclose(f1);
        return 1;
    }

    struct result_image r = image_from_bmp(f1);
    if (r.status != BMF_OK){
        printf("%s", "Error while processing file\n");
        free(r.img.pixels);
        close(f1, f2);
        return 1;
    }
    struct image rotated = rotate(r.img);
    free(r.img.pixels);
    if (!image_to_bmp(&rotated, f2)){
        printf("%s", "Error while writing to file\n");
        close(f1, f2);
        free(rotated.pixels);
        return 1;
    }
    free(rotated.pixels);
    close(f1, f2);
    return 0;

}
