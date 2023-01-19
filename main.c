#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define cap_local 64

// Funcoes associadas ao input local do usuario (que fica no console)
typedef struct CONSOLE{
    char letras[cap_local]; 
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

void build_console(console* v){
    v->tamanho = 0;
    v->letras[cap_local] = '\0';
}

void preenche_console(console* v){
    char c; int i = 0;

    while ((c = getchar()) != 10) {
        v->letras[i] = c;
        i++;
    }
    v->tamanho = i;
}

void reseta_console(console* v){
    v->letras[v->tamanho] = '\0';
    v->tamanho = 0;
}

void build_celula(celula* new){
    //assert(new != NULL);
    new->val = '\0';
    new->next = NULL;
    new->prev = NULL;
}

void frente(){
    celula *atual, *prox;

    if(cursor->cel->next == NULL)
        return;
    
    atual = cursor->cel;
    
    prox = atual->next;
    prox->prev = atual;
        
    if((atual->next)->next != NULL)
        prox->next = atual->next->next;

    cursor->cel = prox;
    cursor->coluna++;
}

void tras(){
    celula *atual, *anterior;

    if(cursor->cel->prev == NULL)
        return;
    
    atual = cursor->cel;
    
    anterior = atual->prev;
    anterior->next = atual;
        
    if((atual->prev)->prev != NULL)
        anterior->prev = atual->prev->prev;

    cursor->cel = anterior;
    cursor->coluna--;
}


void cria_celula_vazia_a_direita(){
    celula* new;
    new = (celula*) malloc(sizeof(celula));
    build_celula(new);
    
    celula* a = cursor->cel;
    
    if(a->next != NULL){
        // da nome a proxima celula 
        celula* b = a->next;
        // atualiza os ponteiros
        a->next   = new;
        new->prev = a;
        new->next = b;
        
        b->prev   = new;
    }
    else{
        a->next   = new;
        new->prev = a;
    }
}

void insert_char_a_direita(char d){
    // celula a direita do cursor
    cria_celula_vazia_a_direita();
    
    // atualiza o cursor
	frente();
	
	// recebe o valor
    (cursor->cel)->val = d;
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
	cursor->cel = tail;

	for(int i=0; i < cons->tamanho; i++){
		insert_char_a_direita(cons->letras[i]);
	    tail = tail->next;
	}
}

void imprime_cursor(){
	for(int i=0;i < cursor->coluna;i++)
		printf(" ");
	printf("^\n");
}

void imprime_coord(){
	printf("%d,%d>", cursor->linha, cursor->coluna);
}

int parse(celula* cel, console* cons) {
    // pega o primeiro caractere da string
    char c = cons->letras[0];

	int n = cons->tamanho;

	console	aux; 
    switch (c) {
    case 'I':
        // lista de celulas auxiliar removendo a cabeca
		(&aux)->tamanho = n - 1;
    	for(int i=0; i< n-1; i++)
			(&aux)->letras[i] = cons->letras[i+1];

		from_console_to_line(&aux);
		break;
	case '!': // sair do programa 
		return 2;
    case 'F': //move cursor pra frente
        frente();
        break;
    case 'T': //move cursor pra tras
        tras();
        break;
    default:
        printf("Comando desconhecido\n");
        break;
    }
    printa_celula();
	imprime_cursor();
	imprime_coord();
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

    int retval;imprime_coord();
    do {
        preenche_console(v);
        retval = parse(cel, v);
		reseta_console(v);
    } while (retval != 2);
    
    return 0;
}

