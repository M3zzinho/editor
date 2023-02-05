#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define NUM_FUNCOES 22
#define NUM_FUNCOES_COMEM_STRING 6
#define cap_local 1024
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

bool check_vec(char x, char *lista, int num_funcoes);

typedef struct LINHA
{
    int tamanho;
    struct CELULA *head;
    struct CELULA *tail;
    struct LINHA *up;
    struct LINHA *down;
} linha;

typedef struct CELULA
{
    char val;
    struct CELULA *next;
    struct CELULA *prev;
} celula;

typedef struct CONSOLE
{
    char *letras;
    int tamanho;
} console;

typedef struct PONTO
{
    int linha;
    int coluna;
    struct CELULA *cel;
} ponto;

int power(int base, int exp);
#endif
