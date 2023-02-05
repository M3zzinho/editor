#ifndef EDITOR_H
#define EDITOR_H
#include "console.h"

char lista_f[NUM_FUNCOES] = {
    'G', 'I', 'D', 'F', 'T', 'C', 'V', 'X',
    'O', '$', 'P', 'Q', '!', 'B', 'N', 'J',
    'H', ':', 'A', 'E', 'U', 'S'};

char lista_f_come_string[NUM_FUNCOES_COMEM_STRING] = {
    'I', 'B', 'S', 'E', 'A', ':'};

void full_print();

void imprime_coord();

void from_console_to_line(console *cons);

int number_no_console(console *d);

void copia_memoria();

void cola_memoria();

void recorta_memoria();

char *console_to_string(console *cons, char *str);

void KMP_prefixo(char *padrao, int M, int *prefixos);

int KMP_busca(console *cons);

bool mecanismo_de_busca(console *cons);

void substitui(console *old, console *new_console);

void performa_busca(console *cons, bool substituir);

void escreve_arquivo(console *cons_input);

void lida_com_texto_ja_escrito();

void le_um_documento(console *cons);

int parse(console *cons);
#endif
