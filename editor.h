#ifndef EDITOR_H
#define EDITOR_H

#define NUM_FUNCOES 22
#define NUM_FUNCOES_COMEM_STRING 6
#define cap_local 1024
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

extern char lista_f[NUM_FUNCOES];
extern char lista_f_come_string[NUM_FUNCOES_COMEM_STRING];

/*Identifica se um valor x esta numa lista de funcoes*/
// bool check_vec(char x, char *lista, int num_funcoes);

#endif // EDITOR_H