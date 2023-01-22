#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>

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
#define NUM_FUNCOES 14
char lista_f[NUM_FUNCOES] = {
    'G', 'I', 'D', 'F', 'T', 'C', 'V', 'X',
    'O', '$', 'P', 'Q', '!', 'B'};

bool check_vec(char x) {
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

typedef struct CELULA{
    char val;
    struct CELULA* next;
    struct CELULA* prev;
} celula;

typedef struct LINHA{
    int tamanho;
    struct CELULA* head;
    struct CELULA* tail;
    struct LINHA* up;
    struct LINHA* down;
} linha;


typedef struct PONTO{
    int linha;
    int coluna;
    struct CELULA* cel;
} ponto;

ponto *cursor;
celula *head, *tail, *ctrl_c, *ctrl_x;
linha* pres_line;


int tipo_de_cursor = 0;

int ctrl_c_coluna = 0;

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

void build_linha(linha* new){
    new->tamanho = 0;
    new->head = NULL;
    new->tail = NULL;
    new->up = NULL;
    new->down = NULL;
}

void cria_celula_vazia_a_direita(){
    celula* new = (celula*) malloc(sizeof(celula));
    if(new == NULL){
        printf("Erro ao alocar memoria para a celula\n");
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
        printf("Erro: cursor vazio\n");
        return;
    }

    if(head==NULL || (cursor->cel)->next == NULL){
        printf("Erro: nao ha celula a frente do cursor\n");
        return;
    }
    cursor->cel = (cursor->cel)->next;
    cursor->coluna++;
}

void traz(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio\n");
        return;
    }
    if(head == NULL || (cursor->cel)->prev == NULL){
        printf("Erro: nao ha celula atras do cursor\n");
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
    cria_celula_vazia_a_direita();
    frente();

    (cursor->cel)->val = d;
    pres_line->tamanho++;
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
    celula* aux = pres_line->head->next;
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

int copia_memoria(){
    if(cursor->cel == NULL)
        return 0;
    ctrl_c = cursor->cel;
    return cursor->coluna;
}

void cola_memoria(){
    if(ctrl_c == NULL)
        return;
    insert_char_a_direita(ctrl_c->val);
}

void recorta_memoria(){
    if(cursor->cel == NULL)
        return;

    celula *atual = cursor->cel;
    int atual_coluna = cursor->coluna;

    if(ctrl_c == NULL)
        return;
    // aponta o cursor pra poder deletar
    cursor->cel = ctrl_c;
    char d = cursor->cel->val;

    delete_char();

    cursor->cel = atual;
    
    // atualiza a coluna do cursor (cuidando pro 
    // caso da exclusao estar antes ou depois do 
    // cursor)
    cursor->coluna = atual_coluna;
    if(atual_coluna > ctrl_c_coluna)
        cursor->coluna--;

    insert_char_a_direita(d);
}

int tamanho_da_linha(){
    int n = 0;
    celula* aux = head->next;
    while(aux != NULL){
        n++;
        aux = aux->next;
    }
    return n;
}

char* line_to_string(){
    int n = tamanho_da_linha();
    char* str = (char*) malloc(n*sizeof(char));
    celula* aux = head->next;
    for(int i=0; i<n; i++){
        str[i] = aux->val;
        aux = aux->next;
    }
    return str;
}

// Implementando o algoritimo de Knuth Morris Pratt 
void KMP_prefixo(char* padrao, int M, int* prefixos) {
    prefixos[0] = 0;
    int j = 0, i=1;
    
    // percorre o padrao
    while(i < M){
        if(padrao[i] == padrao[j]){
            j++;
            prefixos[i] = j;
            i++;
        } else {
            if(j != 0)
                j = prefixos[j-1];
            else{
                prefixos[i] = 0;
                i++;
            }
        }
    }

    // // printa prefixos
    // for(int i=0; i<M; i++)
    //     printf("prefixos[%d] = %d\n", i, prefixos[i]);
}

int KMP_busca(console* cons){
    int M = cons->tamanho;
    int N = pres_line->tamanho - cursor->coluna;
    char padrao[M];

    for(int i=0; i<M; i++)
        padrao[i] = cons->letraz[i];

    int prefixos[M];

    // calcula o prefixo e o sufixo do padrao
    KMP_prefixo(padrao, M, prefixos);

    int j=0, i=0;
    // "texto[i]"
    celula* texto_i = cursor->cel;

    while(i < N){
        if(padrao[j] == texto_i->val){
            j++;i++;
            texto_i = texto_i->next;
        }

        if(j == M)
            return i-j;
        else if(i < N && padrao[j] != texto_i->val){
            if(j != 0)
                j = prefixos[j-1];
            else{
                i++; texto_i = texto_i->next;
            }
        }
    }
    
    return -1;
}

void mecanismo_de_busca(console *cons_input){
    int ind_kmp = KMP_busca(cons_input);

    if(ind_kmp == -1)
        return;
        
    // anda ate o indice encontrado pelo metodo
    while(ind_kmp > 0){
        frente();
        ind_kmp--;
    }
}

int parse(console* cons){
    ClearScreen();

    // pega o primeiro caractere da string
    char c = cons->letraz[0];
    console *cons_input;

    // lista de celulas auxiliar removendo a cabeca para funcoes que usam strings do usuario
    // > Input: 𝑓a₀a₁...aₙ
    // > Output: a₀a₁...aₙ
    if(cons->tamanho > 1){
        cons_input = malloc(sizeof(console));
        build_console(cons_input);
        cons_input->tamanho = cons->tamanho - 1;
        for(int i=0; i< cons_input->tamanho; i++)
            cons_input->letraz[i] = cons->letraz[i+1];
    }

    // numero de iteracoes de uma dada funcao
    int iteradas = number_no_console(cons);

    // uma vez que o numero de iteradas da funcao ja esta aramazenado, removemos ele do console
    int j=0;
    while(isdigit(cons->letraz[j+1])){
        delete_char_console(cons, j+1); j++;
    }

    if(iteradas == 0)
        iteradas = 1;

    bool search_next = false; char s;

    switch(c){
        case 'I':
            if(cons_input != NULL)
                from_console_to_line(cons_input);
            break;
        case 'B':
            // primeiro caso comeca a busca a partir do inicio da linha
            cursor->cel = pres_line->head;
            cursor->coluna = 0;
            
            // do-while executa pelo menos uma vez a busca
            do {
                mecanismo_de_busca(cons_input); 
                printa_celula();
                imprime_cursor();
                imprime_coord();
                printf("\n\n");
                
                printf("Continuar busca? (s/n) ");
                s = getchar();
                if(s == 's')
                    search_next = true;
                else
                    search_next = false;
                printf("\n");
            } while(search_next == true);
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
            if(cursor->cel != pres_line->head){
                cursor->cel = pres_line->head;
                cursor->coluna = 0;
            }
            break;
        case '$':
            if(cursor->cel != pres_line->tail){
                cursor->cel = pres_line->tail;
                cursor->coluna = pres_line->tamanho;
            }
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
        case 'C':
            ctrl_c_coluna = copia_memoria();
            break;
        case 'V':
            cola_memoria();
            break;
        case 'X':
            recorta_memoria();
            break;
        case '!': // sair do programa
            return 2;
        case '\0':
            break;
        default:
            printf("Comando desconhecido\n");
            break;
    }
    if(c != 'B'){ // se for busca, ja imprimiu uma vez
        printa_celula();
        imprime_cursor();
        imprime_coord();
    }

    while(pres_line->tail->next != NULL)
        pres_line->tail = pres_line->tail->next;
    
    // passo recursivo: casos expurios
    if(cons->tamanho > 1){
        if(check_vec(cons->letraz[1]) == false){
            free(cons_input);
            return 0;
        }
        if(cons_input->tamanho == 0){
            free(cons_input);
            return 0;
        }
        if(c != 'I' || c != 'B' || c != 'S')
            parse(cons_input);
        if(cons_input != NULL)
            free(cons_input);
    }
    return 0;
}

int main(){
    // possibilita usar caracteres
    setlocale(LC_ALL, "pt_BR.UTF-8");
    freopen("CON","w",stdout);

    // cursor = (x,y) da coordenada cursor
    cursor = (ponto*) malloc(sizeof(ponto));
    cursor->linha = 0; cursor->coluna = 0;

    // linha original
    pres_line = (linha*) malloc(sizeof(linha));
    build_linha(pres_line);

    console* v;
    v = (console*) malloc(sizeof(console));
    build_console(v);

    head = (celula*) malloc(sizeof(celula));
    tail = (celula*) malloc(sizeof(celula));

    celula* cel;
    cel = (celula*) malloc(sizeof(celula));
    build_celula(cel);

    head = cel;
    tail = cel;

    pres_line->head = head;
    pres_line->tail = tail;

    cursor->cel = cel;

    int retval;
    imprime_coord();
    do {
        preenche_console(v);
        if(v->tamanho == 0)
            ClearScreen();
        retval = parse(v);
        reseta_console(v);
    } while (retval != 2);

    return 0;
}



