#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define cores_maximas 5 
//0, 1, 2, 3, 4 e correspondem às cores 0 0 0-preto, 255 0 0-vermelho, 0 255 0-verde, 0 0 255-azul, 255 255 0-amarelho
typedef struct {
    uint32_t r;
    uint32_t g;
    uint32_t b;
} color_t;

int indice_cor(color_t cor);

void print_comprimir_matriz(uint32_t **comprimir, uint32_t altura, uint32_t largura) {
    for (uint32_t i = 0; i < altura; i++) {
        for (uint32_t j = 0; j < largura; j++) {
            printf("%u", comprimir[i][j]);
            if (j != largura - 1) {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void comprimir_imagem(color_t **pixels, uint32_t altura, uint32_t largura) {
    uint32_t **comprimida = (uint32_t **)malloc(altura * sizeof(uint32_t *));
    if (comprimida == NULL) {
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < altura; i++) {
        comprimida[i] = (uint32_t *)calloc(cores_maximas, sizeof(uint32_t));
        if (comprimida[i] == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    for (uint32_t i = 0; i < altura; i++) {
        uint32_t cont_cor = 0;
        color_t prev_cor = pixels[i][0];
        for (uint32_t j = 0; j < largura; j++) {
            color_t cores = pixels[i][j];
            if (memcmp(&cores, &prev_cor, sizeof(color_t)) == 0) {
                cont_cor++;
            } else {
                comprimida[i][indice_cor(prev_cor)] = cont_cor;
                cont_cor = 1;
                prev_cor = cores;
            }
        }
        comprimida[i][indice_cor(prev_cor)] = cont_cor;
    }

    print_comprimir_matriz(comprimida, altura, cores_maximas);

    for (uint32_t i = 0; i < altura; i++) {
        free(comprimida[i]);
    }
    free(comprimida);
}

int indice_cor(color_t cor) {
    if (cor.r == 0 && cor.g == 0 && cor.b == 0) return 0;
    if (cor.r == 255 && cor.g == 0 && cor.b == 0) return 1;
    if (cor.r == 0 && cor.g == 255 && cor.b == 0) return 2;
    if (cor.r == 0 && cor.g == 0 && cor.b == 255) return 3;
    if (cor.r == 255 && cor.g == 255 && cor.b == 0) return 4;
    if (cor.r == 255 && cor.g == 255 && cor.b == 255) return 5; // Cor branca
    return -1; // Cor inválida
}

color_t cor_indice(int indice) {
    color_t cor = {0};
    
    switch(indice){
        case 0:
            cor.r = 0;
            cor.g = 0;
            cor.b = 0;
            break;

        case 1:
            cor.r = 255;
            cor.g = 0;
            cor.b = 0;
            break;
            
        case 2:
            cor.r = 0;
            cor.g = 255;
            cor.b = 0;
            break;
            
        case 3:
            cor.r = 0;
            cor.g = 0;
            cor.b = 255;
            break;

            
        case 4:
            cor.r = 255;
            cor.g = 255;
            cor.b = 0;
            break;
    }

    return cor;
}

int main() {
    char input[1000]; // Assumindo que a matriz tenha no maximo 1000 caracteres
    fgets(input, sizeof(input), stdin);

    uint32_t altura, largura, max_cores;
    if (sscanf(input, "P3 %u %u %u", &largura, &altura, &max_cores) != 3) {
        return EXIT_FAILURE;
    }

    color_t **pixels = (color_t **)malloc(altura * sizeof(color_t *));
    if (pixels == NULL) {
        return EXIT_FAILURE;
    }

    for (uint32_t i = 0; i < altura; i++) {
        pixels[i] = (color_t *)malloc(largura * sizeof(color_t));
        if (pixels[i] == NULL) {
            return EXIT_FAILURE;
        }
    }

    for (uint32_t i = 0; i < altura; i++) {
        for (uint32_t j = 0; j < largura; j++) {
            if (fscanf(stdin, "%u %u %u", &pixels[i][j].r, &pixels[i][j].g, &pixels[i][j].b) != 3) {
                return EXIT_FAILURE;
            }
        }
    }

    comprimir_imagem(pixels, altura, largura);

    // Encontrar a cor dominante e imprimir a matriz com alto contraste
    int cor_forte[cores_maximas] = {0};
    for (uint32_t i = 0; i < altura; i++) {
        for (uint32_t j = 0; j < largura; j++) {
            cor_forte[indice_cor(pixels[i][j])]++;
        }
    }

    int max_cont_cor = 0;
    int cor_dominantes[cores_maximas] = {0};
    int cor_dominante_size = 0;
    // quantidade da cor mais dominante
    for (int i = 0; i < cores_maximas; i++) {
        if (cor_forte[i] > max_cont_cor) {
            max_cont_cor = cor_forte[i];
        }
    }

    // separar core dominantes
    for (uint32_t i = 0; i < cores_maximas; i++) {
        if(cor_forte[i] >= max_cont_cor){
            cor_dominantes[cor_dominante_size] = i;
            cor_dominante_size++;
        }
    }

    // contraste
    for (uint32_t i = 0; i < altura; i++) {
        for (uint32_t j = 0; j < largura; j++) {
            int index = indice_cor(pixels[i][j]);
            bool dominante = false;
            for(int k = 0; k < cor_dominante_size; k++) {
                if(index == cor_dominantes[k]){
                    dominante = true;
                    break;
                }
            }

            if(dominante){
                // printf("dominante!\n");

                if(cor_dominante_size == 1){
                    // printf("black!\n");
                    printf("0 0 0 ");
                }
                else{
                    color_t cor = cor_indice(index);
                    printf("%u %u %u ", cor.r, cor.g, cor.b);
                }
            } else {
                // printf("negativo dominante!\n");
                printf("255 255 255 ");
            }
        }
        printf("\n");
    }

    for (uint32_t i = 0; i < altura; i++) {
        free(pixels[i]);
    }
    free(pixels);

    return EXIT_SUCCESS;
}