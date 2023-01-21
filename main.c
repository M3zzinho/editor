#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
/* Testes:
1 Inserir uma letra e deletá-la imediatamente
2 Mover o cursor para frente e para trás diversas vezes
3 Inserir várias letras e deletá-las uma a uma
4 Tentar mover o cursor para uma posição que não existe (ex: tentar mover o cursor para uma posição antes do início da lista)
5 Tentar inserir uma letra em uma posição inválida (ex: cursor vazio ou lista vazia)
6 Testar a capacidade máxima do console (cap_local) e verificar se o código está lidando corretamente com essa situação
7 Inserir caracteres especiais ou números para verificar se o código está lidando corretamente com esses casos.
*/


// Funcao para limpar a tela 
/*****************************/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void ClearScreen(){
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
}

#else // !_WIN32
#include <unistd.h>
#include <term.h>

void ClearScreen(){
  if (!cur_term)
  {
     int result;
     setupterm( NULL, STDOUT_FILENO, &result );
     if (result <= 0) return;
  }

   putp( tigetstr( "clear" ) );
}
#endif
/*****************************/

#define cap_local 64

// funcoes implementadas
#define NUM_FUNCOES 6
char lista_f[NUM_FUNCOES] = {'G', 'I', 'D', 'F', 'T', '!'};

bool check_vec(char x, int i) {
    if (x == '\0')
        return false;

    for (int i = 0; i < NUM_FUNCOES; i++) {
        if (lista_f[i] == x) 
            return true;
    }
    return false;
}

// Variaveis globais
/*****************************/
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

// Funcoes do console
/*****************************/
void build_console(console* v){
    v->tamanho = 0;
    v->letraz = (char*) malloc(cap_local * sizeof(char));

    if(v->letraz == NULL){
        printf("Erro ao alocar memoria para o console");
        exit(1);
    }
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

void delete_char_console(console* cons, int i) {
    if (i < 0 || i >= cons->tamanho)
        return;

    for (int j = i; j < cons->tamanho - 1; j++)
        cons->letraz[j] = cons->letraz[j + 1];

    cons->tamanho--;
}

// Funcoes da celula
/*****************************/
void build_celula(celula* new){
    new->val = '\0';
    new->next = NULL;
    new->prev = NULL;
}

void cria_celula_vazia_a_direita(){
    celula* new = (celula*) malloc(sizeof(celula));
    if(new == NULL){
        printf("Erro ao alocar memoria para a celula");
        exit(1);
    }
    
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
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio");
        return;
    }

    if(head==NULL || (cursor->cel)->next == NULL){
        printf("Erro: nao ha celula a frente do cursor");
        return;
    }
    cursor->cel = (cursor->cel)->next;
    cursor->coluna++;
}

void traz(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio");
        return;
    }
    if(head == NULL || (cursor->cel)->prev == NULL){
        printf("Erro: nao ha celula atras do cursor");
        return;
    }    

    cursor->cel = (cursor->cel)->prev;
    cursor->coluna--;
}

void insert_char_a_direita(char d){
    // caso o cursor seja vazio
    if(cursor->cel == NULL){
        celula *new = (celula*) malloc(sizeof(celula));

        if(new == NULL){
            printf("Erro ao alocar memoria para a celula");
            exit(1);
        }

        build_celula(new);

        if((cursor->cel)->next != NULL){
            // da nome a proxima celula
            celula* b = (cursor->cel)->next;
            b->prev = new;
        }   
    }

    // celula a direita do cursor
    cria_celula_vazia_a_direita();frente();

    (cursor->cel)->val = d;
}

void delete_char(){
    // verifica se o cursor e a lista estao vazios
    if(cursor == NULL || head == NULL){
        printf("Erro: cursor ou lista estao vazios");
        return;
    }
    celula *atual; atual = cursor->cel;
    celula *anterior, *proxima;

    // verifica se a celula atual e a head
    if(atual == head){
        head = head->next;
        if(head != NULL) head->prev = NULL;
    }
    // verifica se a celula atual e a tail
    else if(atual == tail){
        tail = tail->prev;
        if(tail != NULL) tail->next = NULL;
    }
    // caso contrario, atualiza as celulas anterior e proxima
    else{
        anterior = atual->prev;
        proxima = atual->next;
        anterior->next = proxima;
        proxima->prev = anterior;
    }

    // move o cursor para a celula anterior
    traz();
    free(atual);
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

int number_no_console(console* d){
    if(d->letraz[1] == '\0' || d->tamanho == 1)
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

void move_cursor_next_palavra(){
    while(cursor->cel != NULL && 
          cursor->cel->val != ' ')
        if(cursor->cel->next != NULL)
            frente();
}

void move_cursor_prev_palavra(){
    while(cursor->cel != NULL && 
          cursor->cel->val != ' ')
        if(cursor->cel->prev != NULL)
            traz();
}

int parse(celula* cel, console* cons) {
    ClearScreen();

    // pega o primeiro caractere da string
    char c = cons->letraz[0];

    int n = cons->tamanho;
    
    // numero de iteracoes de uma dada funcao
    int iteradas = number_no_console(cons);

    // uma vez que o numero de iteradas da funcao ja esta aramazenado, removemos ele do console
    int j=0;
    while(isdigit(cons->letraz[j+1])){
        delete_char_console(cons, j+1);
        j++;
    }

    if(iteradas == 0)
        iteradas = 1;
    
    // lista de celulas auxiliar removendo a cabeca
    console *aux = malloc(sizeof(console));
    build_console(aux);
    aux->tamanho = n - 1;
    for(int i=0; i< n-1; i++)
        aux->letraz[i] = cons->letraz[i+1];

    switch (c) {
        case 'G':
            tipo_de_cursor= (tipo_de_cursor+1) % 2; 
        case 'I':
            from_console_to_line(aux);
            break;
        case 'D':
            while(iteradas > 0){ 
                if(cursor->cel != head)
                    delete_char();
                iteradas--;
            }
            break;
        case 'F': //move cursor pra frente
            while(iteradas > 0){ 
                if((cursor->cel)->next != NULL)
                    frente();
                iteradas--;
            }
            break;
        case 'T': //move cursor pra traz
            while(iteradas > 0){ 
                if((cursor->cel)->prev != NULL)
                    traz();
                iteradas--;
            }
            break;
        case 'O':
            if(cursor->cel != head){
                cursor->cel = head;
                cursor->coluna = 0;
            }
            break;
        case '$':
            while(cursor->cel != tail)
                frente();
            break;
        case 'P':
            while(iteradas > 0){ 
                move_cursor_next_palavra();
                iteradas--;
            }
            break;    
        case 'Q':
            while(iteradas > 0){ 
                move_cursor_prev_palavra();
                iteradas--;
            }
            break;    
        case '!': // sair do programa
            return 2;
        case '\0':
            break;
        default:
            printf("Comando desconhecido\n");
            break;
    }
    printa_celula();
    imprime_cursor();
    imprime_coord();
    
    while(tail->next != NULL)
        tail = tail->next;
    
    // passo recursivo: casos expurios
    if(check_vec(cons->letraz[1], 0) == false){
        free(aux);
        return 0;
    }
    if(aux->tamanho == 0){
        free(aux);
        return 0;
    }
    if(c != 'I'){
        parse(cel, aux);
    }
    if(aux != NULL)
        free(aux);
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



