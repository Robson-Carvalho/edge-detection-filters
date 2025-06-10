#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

void show_menu();
unsigned char* convert_to_grayscale(const char* input_path, int* width, int* height);

int main() {


  int start = 1;
  while (start) {
    int op = 0;
    printf("1 - Programa\n0 - Sair\n\nEscolha uma opcao: ");
    if (scanf("%d", &op) != 1) {
      while (getchar() != '\n');
      printf("Entrada invalida. Por favor, insira um numero.\n");
      continue;
    }

    if (op == 0) {
      printf("Saindo...\n");
      start = 0;
      break;
    }

    if (op != 1) {
      printf("Opcao invalida. Tente novamente.\n");
      continue;
    }

    int width, height;
    char input_path[100];
    printf("\nDigite o nome da imagem (ex: imagem.jpg): ");
    if (scanf("%99s", input_path) != 1) {
      while (getchar() != '\n');
      printf("Erro ao ler o nome da imagem.\n");
      continue;
    }

    char full_path[200];
    snprintf(full_path, sizeof(full_path), "../images/data/%s", input_path);

    unsigned char* gray_img = convert_to_grayscale(full_path, &width, &height);
    if (!gray_img) {
      continue;
    }

    int choice = 0;
    while (choice != 6) {
      show_menu();
      printf("\nEscolha uma opcao: ");
      if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Entrada invalida. Por favor, insira um numero.\n");
        continue;
      }


      stbi_image_free(gray_img);
    }
  }
  return 0;
}

void show_menu() {
  printf("\n=== MENU DE FILTROS ===\n");
  printf("1 - Aplicar Filtro Laplaciano (5x5)\n");
  printf("2 - Aplicar Filtro Prewitt (5x5)\n");
  printf("3 - Aplicar Filtro Roberts (2x2)\n");
  printf("4 - Aplicar Filtro Sobel (3x3)\n");
  printf("5 - Aplicar Filtro Sobel (5x5)\n");
  printf("6 - Voltar ao menu principal\n");
}

unsigned char* convert_to_grayscale(const char* input_path, int* width, int* height) {
  int channels;
  unsigned char* img = stbi_load(input_path, width, height, &channels, 1);

  if (!img) {
    printf("Erro ao carregar a imagem de %s! Verifique o caminho e o formato do arquivo.\n", input_path);
    printf("STB Image Error: %s\n", stbi_failure_reason());
    return NULL;
  }

  printf("Imagem %s carregada: %dpx x %dpx, Canais Originais: %d, Convertida para Escala de Cinza\n", input_path, *width, *height, channels);
  return img;
}
