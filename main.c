#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>

#define cap_local 1024

// funcoes implementadas
#define NUM_FUNCOES 20
char lista_f[NUM_FUNCOES] = {
    'G', 'I', 'D', 'F', 'T', 'C', 'V', 'X',
    'O', '$', 'P', 'Q', '!', 'B', 'N', 'J',
    'H', ':', 'A', 'E'};

bool check_vec(char x) {
    if (x == '\0')
        return false;

    for (int i = 0; i < NUM_FUNCOES; i++) {
        if (lista_f[i] == x) 
            return true;
    }
    return false;
}

// Principais estruturas
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


// Variaveis globais
ponto *cursor;
celula *head, *tail, *ctrl_c, *ctrl_x;
linha* pres_line, *head_line, *tail_line;

int numero_de_linhas = 1;
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
        celula* right = (cursor->cel)->next;
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
        new->next = right;
        right->prev = new;
    } else {
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
    }
}

// movimentos do cursor
void cursor_frente(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio\n");
        return;
    }

    if(pres_line->head==NULL || (cursor->cel)->next == NULL)
        return;

    cursor->cel = (cursor->cel)->next;
    cursor->coluna++;
}

void cursor_traz(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio\n");
        return;
    }
    if(pres_line->head == NULL || 
        (cursor->cel)->prev == NULL)
        return;    

    cursor->cel = (cursor->cel)->prev;
    cursor->coluna--;
}

void cursor_baixo(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio\n");
        return;
    }
    
    if(pres_line->down == NULL)
        return; 

    linha* old_atual = pres_line;
    linha* old_baixo = pres_line->down;
    
    pres_line = old_baixo;
    pres_line->up = old_atual;
    old_atual->down = pres_line;
    

    // coloca o cursor na primeira celula da linha
    cursor->cel = pres_line->head;
    cursor->linha++;
    int old_coluna = cursor->coluna;
    cursor->coluna = 0;

    if(pres_line->head == NULL)
        cria_celula_vazia_a_direita();
    celula* aux = cursor->cel;

    while(aux->next != NULL && old_coluna > 0){
        aux = aux->next;
        cursor_frente();
        old_coluna--;
    }
}

void cursor_cima(){
    if(cursor->cel == NULL){
        printf("Erro: cursor vazio\n");
        return;
    }
    if(pres_line->up == NULL)
        return; 

    linha* old_atual = pres_line;
    linha* old_cima = pres_line->up;
    
    pres_line = old_cima;
    pres_line->down = old_atual;
    old_atual->up = pres_line;
    
    cursor->cel = pres_line->head;
    cursor->linha--;

    int old_coluna = cursor->coluna;
    cursor->coluna = 0;

    if(pres_line->head == NULL)
        cria_celula_vazia_a_direita();
    celula* aux = cursor->cel;
    while (aux->next != NULL && old_coluna > 0)
    {
        aux = aux->next;
        cursor_frente();
        old_coluna--;
    }
    
}

void insert_char_a_direita(char d){
    // caso o cursor seja vazio
    if(cursor->cel == NULL){
        celula *new = (celula*) malloc(sizeof(celula));

        if(new == NULL){
            printf("Erro ao alocar memoria para a celula\n");
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
    cursor_frente();

    (cursor->cel)->val = d;
    pres_line->tamanho++;
}

void insert_line_em_baixo(){
    linha* new_line;
    new_line = (linha*) malloc(sizeof(linha));
    
    // verifica se a linha foi alocada
    if(new_line == NULL){
        printf("Erro ao alocar memoria para a linha\n");
        exit(1);
    }

    build_linha(new_line);

    // verifica se a linha atual nao e a ultima
    if(pres_line->down != NULL){
        linha* below = pres_line->down;
        pres_line->down = new_line;
        new_line->up = pres_line;
        new_line->down = below;
        below->up = new_line;
    } else {
        pres_line->down = new_line;
        new_line->up = pres_line;
    }

    celula* aux;
    aux = (celula*) malloc(sizeof(celula));
    if(aux == NULL){
        printf("Erro ao alocar memoria para a celula\n");
        exit(1);
    }

    build_celula(aux);
    new_line->head = aux;
    new_line->tail = aux;

    // move o cursor para a nova linha
    cursor->cel = new_line->head;
    cursor->linha++;
    cursor->coluna = 0;

    numero_de_linhas++;
    pres_line = new_line;

    // atualiza a ultima linha
    while(tail_line->down != NULL)
        tail_line = tail_line->down;
}

void insert_line_em_cima(){
    linha* new_line;
    new_line = (linha*) malloc(sizeof(linha));

    // verifica se a linha foi alocada
    if(new_line == NULL){
        printf("Erro ao alocar memoria para a linha\n");
        exit(1);
    }

    build_linha(new_line);

    // verifica se a linha atual nao e a primeira
    if(pres_line->up != NULL){
        linha* above = pres_line->up;
        pres_line->up = new_line;
        new_line->down = pres_line;
        new_line->up = above;
        above->down = new_line;
    } else {
        pres_line->up = new_line;
        new_line->down = pres_line;
    }

    celula* aux;
    aux = (celula*) malloc(sizeof(celula));
    if(aux == NULL){
        printf("Erro ao alocar memoria para a nova celula\n");
        exit(1);
    }

    build_celula(aux);
    new_line->head = aux;
    new_line->tail = aux;

    // move o cursor para a nova linha
    cursor->cel = new_line->head;
    cursor->linha--;
    cursor->coluna = 0;

    numero_de_linhas++;
    pres_line = new_line;

    // atualiza a ultima linha
    while(tail_line->down != NULL)
        tail_line = tail_line->down;
}

void delete_line(){
    // verifica se a linha esta vazia
    if(pres_line->head == NULL){
        printf("Erro: linha vazia\n");
        return;
    }

    // verifica se a linha atual nao e a ultima
    if(pres_line->down != NULL){
        linha* below = pres_line->down;
        pres_line->up->down = below;
        below->up = pres_line->up;
    } else
        pres_line->up->down = NULL;

    // move o cursor para a linha anterior
    cursor->cel = pres_line->up->tail;
    cursor->linha--;
    cursor->coluna = pres_line->up->tamanho;

    pres_line = pres_line->up;
    numero_de_linhas--;

    // atualiza a ultima linha
    while(tail_line->down != NULL)
        tail_line = tail_line->down;
}

void delete_char(){
    // verifica se o cursor e a lista estao vazios
    if(cursor == NULL || pres_line->head == NULL){
        printf("Erro: cursor ou lista estao vazios");
        return;
    }
    celula *atual; atual = cursor->cel;
    celula *anterior, *proxima;

    // verificar extremidades
    if(atual == pres_line->head){
        pres_line->head = pres_line->head->next;
        if(pres_line->head != NULL) 
            pres_line->head->prev = NULL;
    } else if(atual == pres_line->tail){
        pres_line->tail = pres_line->tail->prev;
        if(pres_line->tail != NULL) 
            pres_line->tail->next = NULL;
    } else{ // caso contrario, atualiza as celulas anterior e proxima
        anterior = atual->prev;
        proxima = atual->next;
        anterior->next = proxima;
        proxima->prev = anterior;
    }

    // move o cursor para a celula anterior
    cursor_traz();
    pres_line->tamanho--;
    free(atual);
}

void delete_word(){
    // verifica se o cursor e a lista estao vazios
    if(cursor == NULL || pres_line->head == NULL){
        printf("Erro: cursor ou lista estao vazios\n");
        return;
    }
    celula *atual; atual = cursor->cel;
    
    while(cursor->cel->val != ' ' && 
          cursor->cel->prev != NULL){
        delete_char(); 
        pres_line->tamanho--;
    }
}

void print_line(linha* v){
    celula* aux = v->head;
        
    if(aux == NULL){
        printf("\n"); return;
    }
    if(aux->next != NULL)
        aux = aux->next;
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
    printf("(%d,%d) de (%d,%d): ", cursor->linha, cursor->coluna, numero_de_linhas, pres_line->tamanho);
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
            cursor_frente();
}

void move_cursor_prev_palavra(){
    while(cursor->cel != NULL && 
          cursor->cel->val != ' ')
        if(cursor->cel->prev != NULL)
            cursor_traz();
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
    int n = pres_line->tamanho;
    char* str = (char*) malloc(n*sizeof(char));
    celula* aux = pres_line->head->next;
    for(int i=0; i<n; i++){
        str[i] = aux->val;
        aux = aux->next;
    }
    return str;
}

char* console_to_string(console* cons){
    int n = cons->tamanho;
    char* str = (char*) malloc(n*sizeof(char));
    for(int i=0; i<n; i++)
        str[i] = cons->letraz[i];
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

bool mecanismo_de_busca(console *cons_input){
    int ind_kmp = KMP_busca(cons_input);

    if(ind_kmp == -1 && pres_line->down == NULL){
        printf("Linha de baixo vazia\n");
        return false;
    }
    // anda ate o indice encontrado pelo metodo
    while(ind_kmp > 0){
        cursor_frente();
        ind_kmp--;
    }
    return true;
}

void print_all_lines(){
    linha* aux = head_line;
    int n = 0;
    while(aux != NULL){
        if(n == cursor->linha)
            printf(">");
        else
            printf(" ");
        
        print_line(aux);
        if(n == cursor->linha)
            imprime_cursor();

        aux = aux->down;
        n++;
    }
}

void full_print(){
    printf("\n");
    print_all_lines();
    imprime_coord();
} 

void point_to_master_head(){
    cursor->linha = 0;
    cursor->coluna = 0;
    cursor->cel = head_line->head;

    pres_line = head_line;
}

void point_to_master_tail(){
    pres_line = tail_line;
    cursor->cel = pres_line->tail;
    cursor->linha = numero_de_linhas - 1;
    cursor->coluna = pres_line->tamanho - 1;
}

void escreve_arquivo(console* cons_input){
    point_to_master_head();

    // variaveis necessarias pra abrir um arquivo
    char* file_name; FILE *arq;

    file_name = console_to_string(cons_input);

    arq = fopen(file_name, "w");
    if(arq == NULL){
        printf("Erro ao criar o arquivo\n");
        return;
    }

    char* aux;
    for(int i=0; i<numero_de_linhas; i++){
        aux = line_to_string(pres_line);
        fprintf(arq, "%s\n", aux);
        cursor_baixo();
    }
    fclose(arq);
}

void lida_com_texto_ja_escrito(){
    char s ='a'; console* nome_do_arquivo;
    nome_do_arquivo = malloc(sizeof(nome_do_arquivo));

    printf("Deseja salvar o arquivo atual antes de sair?\n");
    while(s != 's' || s != 'n'){
        printf("Digite 's' para salvar ou 'n' para sair sem salvar: ");
        fflush(stdin); // limpa o buffer de entrada
        scanf("%c", &s);
        int d;
        while ((d = getchar()) != '\n' && d != EOF);
    }

    if(s == 's'){
        printf("\nNome do arquivo: ");
        fflush(stdin); // limpa o buffer de entrada
        scanf("%s", nome_do_arquivo->letraz);

        escreve_arquivo(nome_do_arquivo);
    }
                
    point_to_master_tail();
    // limpa a tela
    while(numero_de_linhas > 1)
        delete_line();

    free(nome_do_arquivo);
}

// void open_txt(char* nome){
//     FILE* arq = fopen(nome, "r");
//     if(arq == NULL){
//         printf("Erro ao abrir o arquivo\n");
//         return;
//     }

//     char c;
//     while((c = fgetc(arq)) != EOF){
//         if(c == '\n')
//             nova_linha();
//         else
//             insere_letra(c);
//     }
//     fclose(arq);
// }


int parse(console* cons){
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
    } else 
        cons_input = NULL;

    // numero de iteracoes de uma dada funcao
    int iteradas = 1;
    iteradas = number_no_console(cons);

    int x, abs_dist; // linha to go

    // uma vez que o numero de iteradas da funcao ja esta aramazenado, removemos ele do console
    int j=0;
    while(isdigit(cons->letraz[j+1])){
        delete_char_console(cons, j+1); j++;
    }

    if(iteradas == 0)
        iteradas = 1;

    char s; bool search_next=true;

    // realiza a funcao de acordo com o caractere
    if(isdigit(c)==false){
    switch(c){
        case 'G':
            tipo_de_cursor = (tipo_de_cursor + 1)% 2;
            break;
        case 'H': //vai pra linha anterior
            while(iteradas > 0){ 
                if(pres_line->up != NULL)
                    cursor_cima();
                iteradas--;
            }
            break;
        case ':': // move o cursor para o inicio da linha x
            if(isdigit(cons->letraz[1])){
                x = number_no_console(cons);
                
                // distancia entre atual e x
                abs_dist = abs(x-cursor->linha);
                while (abs_dist >= 0){
                    if(x > cursor->linha)
                        cursor_baixo();
                    else if(x < cursor->linha)
                        cursor_cima();
                    abs_dist--;
                }
                break;
            } else if(cons->letraz[1] == 'F'){
                //vai pra ultima linha
                pres_line = tail_line;
                cursor->cel = pres_line->head;
                cursor->coluna = 0;
                cursor->linha = numero_de_linhas - 1;
            }
            break;
        case 'J':
            while(iteradas > 0){ 
                if(pres_line->down != NULL)
                    cursor_baixo();
                iteradas--;
            }
            break;
        case 'N':
            insert_line_em_baixo();
            break;
        case 'I':
            if(cons_input != NULL)
                from_console_to_line(cons_input);
            break;
        case 'E':
            escreve_arquivo(cons_input);
            break;
        case 'A': // abre um arquivo
            if(head_line->head->next != NULL);
                lida_com_texto_ja_escrito();
            break;
        case 'D':
            if(isdigit(cons_input->letraz[0])){
                while(iteradas > 0){ 
                    delete_char(); 
                    iteradas--;
                }
                break;
            } else if(cons_input->letraz[0] == 'L'){
                delete_line();
                break;
            } else if(cons_input->letraz[0] == 'W'){
                delete_word();
                break;
            }
            break;
        case 'B':
            // primeiro caso comeca a busca a partir do inicio da linha
            cursor->cel = head_line->head->next;
            cursor->coluna = 1;
            cursor->linha = 0;

            pres_line = head_line;

            if(cons_input == NULL)
                break;

            while(search_next == true){
                search_next = mecanismo_de_busca(cons_input); 

                if(cursor->cel->next == NULL && pres_line->down != NULL){
                    cursor_baixo();
                    cursor->cel = pres_line->head->next;
                    cursor->coluna = 1;
                }

                // lidar com caso da ultima celula
                if(cursor->cel->next == NULL && pres_line->down == NULL){
                    if(cursor->cel->val != 
                    cons_input->letraz[0])
                        cursor_traz();

                    search_next = false;break;
                }

                if(search_next == false){
                    cursor_traz();break;
                }

                full_print();
                printf("\n\n");
            
                printf("Continuar busca? (s/n) ");
                fflush(stdin); // limpa o buffer de entrada
                scanf("%c", &s);
                int d;
                while ((d = getchar()) != '\n' && d != EOF);
                printf("\n");

                if(s == 'n')
                    search_next = false;

                if(s == 's' && cursor->cel->next != NULL)
                    cursor_frente();
            }
            break;
        case 'F': //move cursor pra cursor_frente
            while(iteradas > 0){ 
                if((cursor->cel)->next != NULL)
                    cursor_frente();
                iteradas--;
            }
            break;
        case 'T': //move cursor pra cursor_traz
            while(iteradas > 0){ 
                if((cursor->cel)->prev != NULL)
                    cursor_traz();
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
    }
    
    full_print();

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
        if(c != 'I' && c != 'B' && c != 'S')
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

    // linhas principais
    head_line = (linha*) malloc(sizeof(linha));
    tail_line = (linha*) malloc(sizeof(linha));
    pres_line = (linha*) malloc(sizeof(linha));
    build_linha(pres_line);

    head_line = pres_line;
    tail_line = pres_line;

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
    full_print();
    do {
        preenche_console(v);
        retval = parse(v);
        reseta_console(v);
    } while (retval != 2);

    return 0;
}



