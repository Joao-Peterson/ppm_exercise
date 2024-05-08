#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "src/string+.h"
#include "src/data.h"
#include "src/matrix.h"

typedef struct{
    uint32_t r;
    uint32_t g;
    uint32_t b;
}color_t;

typedef struct{
    uint32_t size;
    color_t *colors;
}colorPalette_t;

typedef struct{
	uint32_t h;
	uint32_t w;
	uint32_t colorDepth;
    color_t **pixels;
}image_t;

typedef struct{
	uint32_t h;
	uint32_t w;
    colorPalette_t pallete;
    uint32_t **paletteIndexes;
}compressed_t;

image_t *image_parse(const string *str){
    image_t *img = calloc(1, sizeof(image_t));
    string_ite ite = string_split(str, "\n");

    string header = ite_next(ite);
    string_ite headerIte = string_split(&header, " ");

    // jump first
    string tok = ite_next(headerIte);
    
    // h
    tok = ite_next(headerIte);
    img->h = string_to_uint(&tok, 10);
    
    // w
    tok = ite_next(headerIte);
    img->w = string_to_uint(&tok, 10);

    // colorDepth
    tok = ite_next(headerIte);
    img->colorDepth = string_to_uint(&tok, 10);

    // allocation
    img->pixels = calloc(img->h, sizeof(color_t*));
    for(size_t i = 0; i < img->h; i++)
        img->pixels[i] = calloc(img->w, sizeof(color_t));

    // lines
    uint32_t i = 0;
    foreach(string, line, ite){

        string_ite colorVal = string_split(&line, " ");
        string val;

        // 3 colors on the img->w columns
        for(uint32_t j = 0; j < img->w; j++){
            val = ite_next(colorVal);
            img->pixels[i][j].r = string_to_uint(&val, 10);    
            val = ite_next(colorVal);
            img->pixels[i][j].g = string_to_uint(&val, 10);    
            val = ite_next(colorVal);
            img->pixels[i][j].b = string_to_uint(&val, 10); 
        }

        i++;
    }

    return img;
}

void image_destroy(image_t *img){
    for(size_t i = 0; i < img->h; i++)
        free(img->pixels[i]);

    free(img->pixels);
    free(img);
}

static inline bool color_equal(color_t a, color_t b){
    return 
        a.r == b.r && 
        a.g == b.g && 
        a.b == b.b;
}

int color_index(const colorPalette_t *palette, color_t color){
    for(int i = 0; i < (int)palette->size; i++){
        if(color_equal(palette->colors[i], color))
            return i;     
    }

    return -1;
}

compressed_t *image_compress(const image_t *img, uint32_t paletteSize, const color_t *paletteColors){
    compressed_t *compressed = calloc(1, sizeof(compressed_t));

    compressed->h = img->h;
    compressed->pallete.size = paletteSize;
    compressed->pallete.colors = calloc(paletteSize, sizeof(color_t));
    for(size_t i = 0; i < paletteSize; i++)
        compressed->pallete.colors[i] = paletteColors[i];

    compressed->w = compressed->pallete.size;
    compressed->paletteIndexes = calloc(compressed->h, sizeof(uint32_t*));

    // alloc
    for(size_t i = 0; i < compressed->h; i++)
        compressed->paletteIndexes[i] = calloc(compressed->w, sizeof(uint32_t));

    // indexes
    for(size_t i = 0; i < compressed->h; i++){
        for(size_t j = 0; j < img->w; j++){
            // get color index on the pallete
            uint32_t index = color_index(&compressed->pallete, img->pixels[i][j]);
            // increment pixel amount
            compressed->paletteIndexes[i][index]++;
        }
    }
    
    return compressed;
}


typedef struct{
    uint32_t index;
    uint32_t qty;
}colorQty_t;

cmp_t maxCmp(void *pa, void *pb){
    colorQty_t *a = (colorQty_t*)pa;
    colorQty_t *b = (colorQty_t*)pb;
    return b->qty > a->qty ? cmp_left : cmp_right; 
}

void image_print(const image_t *img){
    for(size_t i = 0; i < img->h; i++){
        for(size_t j = 0; j < img->w; j++){
            if(j != 0)
                printf(" ");
            printf("%u %u %u", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
        }

        printf("\n");
    }
}   

void compressed_destroy(compressed_t *compressed){
    for(size_t i = 0; i < compressed->h; i++)
        free(compressed->paletteIndexes[i]);

    free(compressed->paletteIndexes);
    free(compressed);
}

void compressed_print(const compressed_t *compressed){
    for(size_t i = 0; i < compressed->h; i++){
        for(size_t j = 0; j < compressed->w; j++){
            if(j != 0)
                printf(" ");
            printf("%u", compressed->paletteIndexes[i][j]);
        }

        printf("\n");
    }
}   

void image_contrast(image_t *img, uint32_t paletteSize, const color_t *paletteColors){
    compressed_t *compressed = image_compress(img, paletteSize, paletteColors);

    uint32_t *colorsSum = calloc(paletteSize, sizeof(uint32_t));

    for(size_t i = 0; i < compressed->h; i++)
        for(size_t j = 0; j < paletteSize; j++)
            colorsSum[j] += compressed->paletteIndexes[i][j];

    list_t *maxes = list_new_custom_queue(true, maxCmp);

    for(size_t j = 0; j < paletteSize; j++){
        colorQty_t *q = malloc(sizeof(colorQty_t));
        q->index = j;
        q->qty = colorsSum[j];
        list_push(maxes, q);
    }   

    uint32_t *colorsAlter = calloc(paletteSize, sizeof(uint32_t));
    uint32_t colorsAlterSize = 0;
    uint32_t lastQty = 0;
    list_ite ll = list_iterate(maxes);
    foreach(colorQty_t*, q, ll){
        if(lastQty == 0)
            lastQty = q->qty;
        else if(q->qty != lastQty)
            break;

        colorsAlter[colorsAlterSize] = q->index;
        colorsAlterSize++; 
    }

    for(size_t i = 0; i < img->h; i++){
        for(size_t j = 0; j < img->w; j++){
            bool altered = false;
            for(size_t k = 0; k < colorsAlterSize; k++){
                if(!color_equal(img->pixels[i][j], paletteColors[colorsAlter[k]])){
                    img->pixels[i][j] = (color_t){.r =255, .g = 255, .b = 255};
                    altered = true;
                    break;
                }
            }

            if(!altered && colorsAlterSize == 1){
                img->pixels[i][j] = (color_t){.r = 0, .g = 0, .b = 0};
            }
        }
    }

    free(colorsSum);
    free(colorsAlter);
    list_destroy(maxes);
    compressed_destroy(compressed);
}

int main(int argc, char **argv){
    string *data;
	if(argc > 1) data = string_from_filename(argv[1], NULL);	// from arg filename
	else         data = string_from_file(stdin, NULL);      	// from pipe
	if(data == NULL) return -1;
	
    image_t *img = image_parse(data);

    const color_t palette[] = {
        {.r = 0,   .g = 0,   .b = 0  },
        {.r = 255, .g = 0,   .b = 0  },
        {.r = 0,   .g = 255, .b = 0  },
        {.r = 0,   .g = 0,   .b = 255},
        {.r = 255, .g = 255, .b = 0  }
    };

    compressed_t *compressed = image_compress(img, 5, palette);

    printf("compressed:\n");
    compressed_print(compressed);

    image_contrast(img, 5, palette);
    printf("Contrast:\n");
    image_print(img);

    compressed_destroy(compressed);
    image_destroy(img);
    string_destroy(data);
	return 0;
}