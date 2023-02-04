#ifndef EDITOR_H
#define EDITOR_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h> // nao da pra usar isso no editor online???
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

char lista_f[NUM_FUNCOES] = {
    'G', 'I', 'D', 'F', 'T', 'C', 'V', 'X',
    'O', '$', 'P', 'Q', '!', 'B', 'N', 'J',
    'H', ':', 'A', 'E', 'U', 'S'};

char lista_f_come_string[NUM_FUNCOES_COMEM_STRING] = {
    'I', 'B', 'S', 'E', 'A', ':'};

bool check_vec(char x, char *lista, int num_funcoes);

// Principais estruturas
typedef struct CONSOLE
{
    char *letras;
    int tamanho;
} console;

typedef struct CELULA
{
    char val;
    struct CELULA *next;
    struct CELULA *prev;
} celula;

typedef struct LINHA
{
    int tamanho;
    struct CELULA *head;
    struct CELULA *tail;
    struct LINHA *up;
    struct LINHA *down;
} linha;

typedef struct PONTO
{
    int linha;
    int coluna;
    struct CELULA *cel;
} ponto;

// Funcoes do console
void build_console(console *v);

void build_ponto(ponto *new_ponto);

void preenche_console(console *v);

void reseta_console(console *v);

void delete_char_console(console *cons, int i);

// Funcoes de celula
void build_celula(celula *new_celula);

void build_linha(linha *new_linha);

void cria_celula_vazia_a_direita();

void point_to_master_head();

void point_to_master_tail();

void cursor_frente();

void cursor_tras();

void cursor_cima();

void cursor_baixo();

void delete_char();

void delete_line();

void delete_word();

void print_line(linha *v);

void imprime_cursor();

void imprime_coord();

void print_all_lines();

void full_print();

celula *marca_posicao_atual();

void volta_pra_posicao_marcada(celula *marcada, int linha, int coluna);

void insert_char_a_direita(char d);

void insert_line_em_baixo();

void conecta_linhas();

void from_console_to_line(console *cons);

int number_no_console(console *d);

void move_cursor_next_palavra();

void move_cursor_prev_palavra();

void copia_memoria();

void cola_memoria();

void recorta_memoria();

int tamanho_da_linha();

char *line_to_string(linha *l);

char *console_to_string(console *cons, char *str);

void KMP_prefixo(char *padrao, int M, int *prefixos);

int KMP_busca(console *cons);

bool mecanismo_de_busca(console *cons);

void substitui(console *old, console *new_console);

void performa_busca(console *cons, bool substituir);

void escreve_arquivo(console *cons_input);

void lida_com_texto_ja_escrito();

void le_um_documento(console *cons);

void move_cursor_to_line(console *cons);

int parse(console *cons);

int power(int base, int exp);
#endif
