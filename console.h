#ifndef CONSOLE_H
#define CONSOLE_H
#include "common.h"

void build_linha(linha *new_linha);

void delete_line();

void print_line(linha *v);

void print_all_lines();

void insert_line_em_baixo();

void conecta_linhas();

int tamanho_da_linha();

char *line_to_string(linha *l);

void build_celula(celula *new_celula);

void cria_celula_vazia_a_direita();

void insert_char_a_direita(char d);

void point_to_master_head();

void point_to_master_tail();

void cursor_frente();

void cursor_tras();

void cursor_cima();

void cursor_baixo();

void delete_char();

void delete_word();

void move_cursor_next_palavra();

void move_cursor_prev_palavra();

int number_no_console(console *d);

void move_cursor_to_line(console *cons);

celula *marca_posicao_atual();

void volta_pra_posicao_marcada(celula *marcada, int linha, int coluna);

void imprime_cursor();

void build_console(console *v);

void build_ponto(ponto *new_ponto);

void preenche_console(console *v);

void reseta_console(console *v);

void delete_char_console(console *cons, int i);
#endif
