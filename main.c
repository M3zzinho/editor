#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define cap_local 64

// Funcoes associadas ao input local do usuario (que fica no console)
typedef struct CONSOLE{
    char *letraz;
    int tamanho;
} console;

typedef struct PONTO{
    int linha;
    int coluna;
    struct CELULA* cel;
} ponto;

typedef struct CELULA{
    char val;
    struct CELULA* next;
    struct CELULA* prev;
} celula;

ponto *cursor;
celula *head, *tail;

int tipo_de_cursor = 0;

void build_console(console* v){
    v->tamanho = 0;
    v->letraz = malloc(cap_local * sizeof(char));
    v->letraz[cap_local - 1] = '\0';
}

void preenche_console(console* v){
    char c; int i = 0;

    while ((c = getchar()) != 10) {
        v->letraz[i] = c;
        i++;
    }
    v->tamanho = i;
}

void reseta_console(console* v){
    v->letraz[v->tamanho] = '\0';
    v->tamanho = 0;
}

void build_celula(celula* new){
    new->val = '\0';
    new->next = NULL;
    new->prev = NULL;
}

void cria_celula_vazia_a_direita(){
    celula* new = (celula*) malloc(sizeof(celula));
    build_celula(new);

    if((cursor->cel)->next != NULL){
        // da nome a proxima celula
        celula* b = (cursor->cel)->next;
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
        new->next = b;
        b->prev = new;
    } else {
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
    }
}

void frente(){
    if((cursor->cel)->next == NULL)
        return;
    cursor->cel = (cursor->cel)->next;
    cursor->coluna++;
}

void traz(){
    if((cursor->cel)->prev == NULL)
        return;
        
    cursor->cel = (cursor->cel)->prev;
    cursor->coluna--;
}

void insert_char_a_direita(char d){
    // celula a direita do cursor
    cria_celula_vazia_a_direita();

    // atualiza o cursor
    frente();

    // recebe o valor
    (cursor->cel)->val = d;
}

void delete_char(){
    celula *atual; atual=cursor->cel;
    celula *anterior, *proxima;
    
    anterior=atual; proxima=atual;
    
    if(atual == tail)
        tail = tail->prev;
    if(atual->prev != NULL)
        anterior = atual->prev;
    if(atual->next != NULL)
        proxima  = atual->next;

    free(atual);  
    
    if(proxima == NULL)
        proxima = tail;
    if(anterior == NULL)
        anterior = head;
        
    atual = anterior;
    atual->prev = anterior->prev;
    atual->next = proxima;
    
    proxima->prev = atual;
    
    // if(atual == head)

    cursor->cel = atual;
    cursor->coluna--;
}

void printa_celula(){
    printf(" ");
    celula* aux = head->next;
    while(aux != NULL){
        printf("%c", aux->val);
        aux = aux->next;
    }
    printf("\n");
}

void from_console_to_line(console* cons){
    for(int i=0; i < cons->tamanho; i++)
	       insert_char_a_direita(cons->letraz[i]);
}

void imprime_cursor(){
    for(int i=0;i < cursor->coluna;i++)
        printf(" ");
    if(tipo_de_cursor==0)
        printf("^\n");
    else
        printf("/\\\n");
}

void imprime_coord(){
    printf("%d,%d>", cursor->linha, cursor->coluna);
}

int pow(int a, int b){
    int p = 1;
    
    while(b > 0){
        p = p*a; b--;
    }
    
    return p;
}

int number_no_console(console* d){
    if(d->letraz[1] == '\0')
        return 1;
    
    int S = 0;
    int n = d->tamanho;
    
    for(int i=1; i<n; i++){
        char x=d->letraz[n-i];
        if(isdigit(x)==0)
            break;
        S = pow(10,i-1)*(x-'0') + S;
    } 
    return S;
}


int parse(celula* cel, console* cons) {
    // pega o primeiro caractere da string
    char c = cons->letraz[0];

    int n = cons->tamanho;
    int iteradas;
    
    console *aux = malloc(sizeof(console));
    build_console(aux);
    switch (c) {
    case 'Z':
        tipo_de_cursor= (tipo_de_cursor+1) % 2; 
    case 'I':
    // lista de celulas auxiliar removendo a cabeca
    aux->tamanho = n - 1;
    for(int i=0; i< n-1; i++)
        aux->letraz[i] = cons->letraz[i+1];

	    from_console_to_line(aux);
	    break;
    case 'D':
        delete_char();
    case 'F': //move cursor pra frente
    iteradas = 
      number_no_console(cons);
    while(iteradas > 0){ 
        if((cursor->cel)->next != NULL)
            frente();
        iteradas--;
    }
	    break;
    case 'T': //move cursor pra traz
    iteradas = 
      number_no_console(cons);
    
    while(iteradas > 0){ 
        if((cursor->cel)->prev != NULL)
            traz();
        iteradas--;
    }
    break;
    case '!': // sair do programa
        return 2;
    default:
    printf("Comando desconhecido\n");
    break;
    }
    printa_celula();
    imprime_cursor();
    imprime_coord();
    
    while(tail->next != NULL)
        tail = tail->next;
    
    return 0;
}

int main(){
    // cursor = (x,y) da coordenada cursor
    cursor = (ponto*) malloc(sizeof(ponto));
    cursor->linha = 0; cursor->coluna = 0;

    // celula original
    celula* cel;
    cel = (celula*) malloc(sizeof(celula));
    build_celula(cel);

    console* v;
    v = (console*) malloc(sizeof(console));
    build_console(v);

    head = cel;
    tail = cel;

    cursor->cel = cel;

    int retval;
    imprime_coord();
    do {
        preenche_console(v);
        retval = parse(cel, v);
        reseta_console(v);
    } while (retval != 2);

    return 0;
}

