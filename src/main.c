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
void apply_filter(unsigned char* gray_img, int width, int height, const char* output_path, int filter_type);
void apply_laplacian(unsigned char* gray_img, unsigned char* output, int width, int height);
void apply_prewitt(unsigned char* gray_img, unsigned char* output, int width, int height);
void apply_roberts(unsigned char* gray_img, unsigned char* output, int width, int height);
void apply_sobel_3x3(unsigned char* gray_img, unsigned char* output, int width, int height);
void apply_sobel_5x5(unsigned char* gray_img, unsigned char* output, int width, int height);
unsigned char* convert_to_grayscale(const char* input_path, int* width, int* height);



void driver(int8_t* kernel, int8_t* region, uint8_t* result, int size, int opcode);

// extern void mmap_setup();
// extern void driver(int8_t *matrixA, int8_t *matrixB, uint8_t *matrixR, int size, int op_opcode);
// extern void mmap_cleanup();

int main() {
  //  mmap_setup();

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

      char ouput_path[200];

      switch (choice) {
      case 1:
        snprintf(ouput_path, sizeof(ouput_path), "../images/output/laplacian_%s", input_path);

        apply_filter(gray_img, width, height, ouput_path, 1);
        printf("Filtro Laplaciano aplicado e salvo como output_laplacian.png\n");
        break;
      case 2:
        snprintf(ouput_path, sizeof(ouput_path), "../images/output/prewitt_%s", input_path);

        apply_filter(gray_img, width, height, ouput_path, 2);
        printf("Filtro Prewitt (5x5) aplicado e salvo como output_prewitt.png\n");
        break;
      case 3:
        snprintf(ouput_path, sizeof(ouput_path), "../images/output/roberts_%s", input_path);

        apply_filter(gray_img, width, height, ouput_path, 3);
        printf("Filtro Roberts (2x2) aplicado e salvo como output_roberts.png\n");
        break;
      case 4:
        snprintf(ouput_path, sizeof(ouput_path), "../images/output/sobel3x3_%s", input_path);

        apply_filter(gray_img, width, height, ouput_path, 4);
        printf("Filtro Sobel (3x3) aplicado e salvo como output_sobel3x3.png\n");
        break;
      case 5:
        snprintf(ouput_path, sizeof(ouput_path), "../images/output/sobel5x5_%s", input_path);

        apply_filter(gray_img, width, height, ouput_path, 5);
        printf("Filtro Sobel (5x5) aplicado e salvo como output_sobel5x5.png\n");
        break;
      case 6:
        printf("Retornando ao menu principal...\n\n");
        break;
      default:
        printf("Opcao invalida! Tente novamente.\n");
      }
    }
    stbi_image_free(gray_img);
  }

  //mmap_cleanup();
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

void apply_filter(unsigned char* gray_img, int width, int height, const char* output_path, int filter_type) {
  unsigned char* output = (unsigned char*)malloc(width * height * sizeof(unsigned char));

  if (!output) {
    printf("Erro: Falha ao alocar memoria para a imagem de saida!\n");
    return;
  }

  memcpy(output, gray_img, width * height * sizeof(unsigned char));

  if (filter_type == 1) {
    apply_laplacian(gray_img, output, width, height);
  }
  else if (filter_type == 2) {
    apply_prewitt(gray_img, output, width, height);
  }
  else if (filter_type == 3) {
    apply_roberts(gray_img, output, width, height);
  }
  else if (filter_type == 4) {
    apply_sobel_3x3(gray_img, output, width, height);
  }
  else if (filter_type == 5) {
    apply_sobel_5x5(gray_img, output, width, height);
  }

  if (!stbi_write_jpg(output_path, width, height, 1, output, 90)) {
    printf("Erro ao salvar a imagem filtrada em %s!\n", output_path);
  }
  free(output);
}


void driver(int8_t* kernel, int8_t* region, uint8_t* result, int size, int opcode) {
  int sum = 0;
  int kernel_dim = 0;

  if (size == 0) { kernel_dim = 2; }
  else if (size == 1) { kernel_dim = 3; }
  else if (size == 3) { kernel_dim = 5; }

  if (kernel_dim > 0) {
    for (int i = 0; i < kernel_dim; i++) {
      for (int e = 0; e < kernel_dim; e++) {
        sum += kernel[i * kernel_dim + e] * region[i * kernel_dim + e];
      }
    }
  }

  if (opcode == 2) {
    if (sum >= 127) {
      result[0] = 127;
    }
    else if (sum <= 0) {
      result[0] = 0;
    }
    else {
      result[0] = sum;
    }
  }
  else {
    sum = (uint16_t)sum;
    result[0] = (sum >> 8) & 0xFF;
    result[1] = sum & 0xFF;
  }
}

void apply_laplacian(unsigned char* gray_img, unsigned char* output, int width, int height) {
  int8_t laplacian_kernel[25] = {
      0,  0, -1,  0,  0,
      0, -1, -2, -1,  0,
     -1, -2, 16, -2, -1,
      0, -1, -2, -1,  0,
      0,  0, -1,  0,  0
  };
  int kernel_size = 5;
  int half_kernel = kernel_size / 2;

  for (int y = half_kernel; y < height - half_kernel; y++) {
    for (int x = half_kernel; x < width - half_kernel; x++) {
      // CORRIGIDO: Usa o método de conversão padrão (- 128)
      int8_t region_s8[25];
      int idx = 0;
      for (int ky = -half_kernel; ky <= half_kernel; ky++) {
        for (int kx = -half_kernel; kx <= half_kernel; kx++) {
          region_s8[idx++] = (int8_t)(gray_img[(y + ky) * width + (x + kx)] - 128);
        }
      }

      uint8_t result_bytes[25];

      driver(laplacian_kernel, region_s8, result_bytes, 3, 2); // Opcode não é mais especial

      output[y * width + x] = (unsigned char)result_bytes[0];
    }
  }
}
void apply_prewitt(unsigned char* gray_img, unsigned char* output, int width, int height) {
  int8_t prewitt_x_kernel[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
  int8_t prewitt_y_kernel[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
  int kernel_size = 3;
  int half_kernel = kernel_size / 2;

  for (int y = half_kernel; y < height - half_kernel; y++) {
    for (int x = half_kernel; x < width - half_kernel; x++) {
      int8_t region_s8[9];
      int idx = 0;
      for (int ky = -half_kernel; ky <= half_kernel; ky++) {
        for (int kx = -half_kernel; kx <= half_kernel; kx++) {
          region_s8[idx++] = (int8_t)(gray_img[(y + ky) * width + (x + kx)] - 128);
        }
      }

      // Arrays para receber os bytes de resultado da driver
      // Mesmo que a driver só use as posições 0 e 1, o array precisa existir
      uint8_t gx_result_bytes[9];
      uint8_t gy_result_bytes[9];

      driver(prewitt_x_kernel, region_s8, gx_result_bytes, 1, 1);
      driver(prewitt_y_kernel, region_s8, gy_result_bytes, 1, 1);

      // --- NOVA LÓGICA DE RECONSTRUÇÃO ---
      // 1. Junte os dois bytes de Gx em um uint16_t (sem sinal)
      uint16_t unsigned_gx = (uint16_t)(gx_result_bytes[0] << 8) | gx_result_bytes[1];
      // 2. Junte os dois bytes de Gy em um uint16_t (sem sinal)
      uint16_t unsigned_gy = (uint16_t)(gy_result_bytes[0] << 8) | gy_result_bytes[1];

      // 3. Converta (cast) o padrão de bits para int16_t (com sinal)
      int16_t gx_final = (int16_t)unsigned_gx;
      int16_t gy_final = (int16_t)unsigned_gy;

      // Agora gx_final e gy_final contêm os valores corretos com sinal
      int magnitude = (int)sqrt(pow(gx_final, 2) + pow(gy_final, 2));

      if (magnitude > 255) magnitude = 255;
      if (magnitude < 0) magnitude = 0;

      output[y * width + x] = (unsigned char)magnitude;
    }
  }
}
void apply_roberts(unsigned char* gray_img, unsigned char* output, int width, int height) {
  int8_t roberts_x_kernel[4] = { 1, 0, 0, -1 };
  int8_t roberts_y_kernel[4] = { 0, 1, -1, 0 };
  int kernel_size = 2;

  for (int y = 0; y < height - 1; y++) {
    for (int x = 0; x < width - 1; x++) {
      int8_t region_s8[4];
      int idx = 0;
      for (int ky = 0; ky < kernel_size; ky++) {
        for (int kx = 0; kx < kernel_size; kx++) {
          region_s8[idx++] = (int8_t)(gray_img[(y + ky) * width + (x + kx)] - 128);
        }
      }

      uint8_t gx_result_bytes[4];
      uint8_t gy_result_bytes[4];

      driver(roberts_x_kernel, region_s8, gx_result_bytes, 0, 1);
      driver(roberts_y_kernel, region_s8, gy_result_bytes, 0, 1);

      uint16_t unsigned_gx = (uint16_t)(gx_result_bytes[0] << 8) | gx_result_bytes[1];
      uint16_t unsigned_gy = (uint16_t)(gy_result_bytes[0] << 8) | gy_result_bytes[1];

      int16_t gx_final = (int16_t)unsigned_gx;
      int16_t gy_final = (int16_t)unsigned_gy;

      int magnitude = (int)sqrt(pow(gx_final, 2) + pow(gy_final, 2));

      if (magnitude > 255) magnitude = 255;
      if (magnitude < 0) magnitude = 0;

      output[y * width + x] = (unsigned char)magnitude;
    }
  }
}
void apply_sobel_3x3(unsigned char* gray_img, unsigned char* output, int width, int height) {
  int8_t sobel_x_kernel[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
  int8_t sobel_y_kernel[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
  int kernel_size = 3;
  int half_kernel = kernel_size / 2;

  for (int y = half_kernel; y < height - half_kernel; y++) {
    for (int x = half_kernel; x < width - half_kernel; x++) {
      int8_t region_s8[9];
      int idx = 0;
      for (int ky = -half_kernel; ky <= half_kernel; ky++) {
        for (int kx = -half_kernel; kx <= half_kernel; kx++) {
          region_s8[idx++] = (int8_t)(gray_img[(y + ky) * width + (x + kx)] - 128);
        }
      }

      uint8_t gx_result_bytes[9];
      uint8_t gy_result_bytes[9];

      driver(sobel_x_kernel, region_s8, gx_result_bytes, 1, 1);
      driver(sobel_y_kernel, region_s8, gy_result_bytes, 1, 1);

      uint16_t unsigned_gx = (uint16_t)(gx_result_bytes[0] << 8) | gx_result_bytes[1];
      uint16_t unsigned_gy = (uint16_t)(gy_result_bytes[0] << 8) | gy_result_bytes[1];

      int16_t gx_final = (int16_t)unsigned_gx;
      int16_t gy_final = (int16_t)unsigned_gy;

      int magnitude = (int)sqrt(pow(gx_final, 2) + pow(gy_final, 2));

      if (magnitude > 255) magnitude = 255;
      if (magnitude < 0) magnitude = 0;

      output[y * width + x] = (unsigned char)magnitude;
    }
  }
}
void apply_sobel_5x5(unsigned char* gray_img, unsigned char* output, int width, int height) {
  int8_t sobel_x_kernel[25] = {
      -2, -4, 0, 4, 2,
      -4, -8, 0, 8, 4,
      -6, -12, 0, 12, 6,
      -4, -8, 0, 8, 4,
      -2, -4, 0, 4, 2
  };
  int8_t sobel_y_kernel[25] = {
      -2, -4, -6, -4, -2,
      -4, -8, -12, -8, -4,
       0,  0,  0,  0,  0,
       4,  8, 12,  8,  4,
       2,  4,  6,  4,  2
  };
  int kernel_size = 5;
  int half_kernel = kernel_size / 2;

  for (int y = half_kernel; y < height - half_kernel; y++) {
    for (int x = half_kernel; x < width - half_kernel; x++) {
      int8_t region_s8[25];
      int idx = 0;
      for (int ky = -half_kernel; ky <= half_kernel; ky++) {
        for (int kx = -half_kernel; kx <= half_kernel; kx++) {
          region_s8[idx++] = (int8_t)(gray_img[(y + ky) * width + (x + kx)] - 128);
        }
      }

      uint8_t gx_result_bytes[25];
      uint8_t gy_result_bytes[25];

      driver(sobel_x_kernel, region_s8, gx_result_bytes, 3, 1);
      driver(sobel_y_kernel, region_s8, gy_result_bytes, 3, 1);

      uint16_t unsigned_gx = (uint16_t)(gx_result_bytes[0] << 8) | gx_result_bytes[1];
      uint16_t unsigned_gy = (uint16_t)(gy_result_bytes[0] << 8) | gy_result_bytes[1];

      int16_t gx_final = (int16_t)unsigned_gx;
      int16_t gy_final = (int16_t)unsigned_gy;

      int magnitude = (int)sqrt(pow(gx_final, 2) + pow(gy_final, 2));

      // CORRIGIDO: Escala a magnitude para reduzir o " estouro"
      // Deslocar 3 bits para a direita é o mesmo que dividir por 8.
      // Isso ajuda a trazer os valores altos para a faixa de 0-255.
      magnitude = magnitude >> 3;

      if (magnitude > 255) magnitude = 255;
      if (magnitude < 0) magnitude = 0;

      output[y * width + x] = (unsigned char)magnitude;
    }
  }
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
