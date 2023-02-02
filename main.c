#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#include <string.h>
#include <windows.h>

#define cap_local 1024

// funcoes implementadas
#define NUM_FUNCOES 20
char lista_f[NUM_FUNCOES] = {
    'G', 'I', 'D', 'F', 'T', 'C', 'V', 'X',
    'O', '$', 'P', 'Q', '!', 'B', 'N', 'J',
    'H', ':', 'A', 'E'};

#define NUM_FUNCOES_COMEM_STRING 6
char lista_f_come_string[NUM_FUNCOES_COMEM_STRING] = {
    'I', 'B', 'S', 'E', 'A', ':'};

bool check_vec(char x, char *lista, int num_funcoes)
{
    if (x == '\0')
        return false;

    for (int i = 0; i < num_funcoes; i++)
    {
        if (lista[i] == x)
            return true;
    }
    return false;
}

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

// Variaveis globais
ponto *cursor;
celula *head, *tail, *ctrl_c, *ctrl_x;
linha *pres_line, *head_line, *tail_line;

int numero_de_linhas = 1;
int tipo_de_cursor = 0;
int ctrl_c_coluna = 0;

// Funcoes do console
/*****************************/
void build_console(console *v)
{
    v->tamanho = 0;
    v->letras = (char *)malloc(cap_local * sizeof(char));

    if (v->letras == NULL)
    {
        printf("Erro ao alocar memoria para o console");
        exit(1);
    }
    v->letras[cap_local - 1] = '\0';
}

void preenche_console(console *v)
{
    char c;
    int i = 0;

    while ((c = getchar()) != 10)
    {
        v->letras[i] = c;
        i++;
    }
    v->letras[i] = '\0';
    v->tamanho = i;
}

void reseta_console(console *v)
{
    v->letras[v->tamanho] = '\0';
    v->tamanho = 0;
}

void delete_char_console(console *cons, int i)
{
    if (i < 0 || i >= cons->tamanho)
        return;

    for (int j = i; j < cons->tamanho - 1; j++)
        cons->letras[j] = cons->letras[j + 1];

    cons->letras[cons->tamanho - 1] = '\0';
    cons->tamanho--;
}

// Funcoes da celula
/*****************************/
void build_celula(celula *new)
{
    new->val = '\0';
    new->next = NULL;
    new->prev = NULL;
}

void build_linha(linha *new)
{
    new->tamanho = 0;
    new->head = NULL;
    new->tail = NULL;
    new->up = NULL;
    new->down = NULL;
}

void cria_celula_vazia_a_direita()
{
    celula *new = (celula *)malloc(sizeof(celula));
    if (new == NULL)
    {
        printf("Erro ao alocar memoria para a celula\n");
        exit(1);
    }

    build_celula(new);

    if ((cursor->cel)->next != NULL)
    {
        // da nome a proxima celula
        celula *right = (cursor->cel)->next;
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
        new->next = right;
        right->prev = new;
    }
    else
    {
        (cursor->cel)->next = new;
        new->prev = cursor->cel;
    }
}

void point_to_master_head()
{
    // while (head_line->up != NULL)
    //     head_line = head_line->up;
    // while (head_line->head->prev != NULL)
    //     head_line->head = head_line->head->prev;

    pres_line = head_line;

    cursor->cel = pres_line->head;
    cursor->linha = 0;
    cursor->coluna = 0;
}

void point_to_master_tail()
{
    while (tail_line->down != NULL)
        tail_line = tail_line->down;
    while (tail_line->tail->next != NULL)
        tail_line->tail = tail_line->tail->next;

    pres_line = tail_line;

    cursor->cel = pres_line->tail;
    cursor->linha = numero_de_linhas - 1;
    cursor->coluna = pres_line->tamanho;
}

// movimentos do cursor
void cursor_frente()
{
    if (cursor->cel == NULL)
    {
        printf("Erro: cursor vazio\n");
        return;
    }

    if (pres_line->head == NULL || (cursor->cel)->next == NULL)
        return;

    cursor->cel = (cursor->cel)->next;
    cursor->coluna++;
}

void cursor_traz()
{
    if (cursor->cel == NULL)
    {
        printf("Erro: cursor vazio\n");
        return;
    }
    if (pres_line->head == NULL ||
        (cursor->cel)->prev == NULL)
        return;

    cursor->cel = (cursor->cel)->prev;
    cursor->coluna--;
}

void cursor_baixo()
{
    if (cursor->cel == NULL)
    {
        printf("Erro: cursor vazio\n");
        point_to_master_head();
        return;
    }

    if (pres_line->down == NULL)
        return;

    linha *old_atual = pres_line;
    linha *old_baixo = pres_line->down;

    pres_line = old_baixo;
    pres_line->up = old_atual;
    old_atual->down = pres_line;

    // coloca o cursor na primeira celula da linha
    cursor->cel = pres_line->head;
    cursor->linha++;
    int old_coluna = cursor->coluna;
    cursor->coluna = 0;

    if (pres_line->head == NULL)
        cria_celula_vazia_a_direita();
    celula *aux = cursor->cel;

    while (aux->next != NULL && old_coluna > 0)
    {
        aux = aux->next;
        cursor_frente();
        old_coluna--;
    }
}

void cursor_cima()
{
    if (cursor->cel == NULL)
    {
        printf("Erro: cursor vazio\n");
        return;
    }
    if (pres_line->up == NULL)
        return;

    linha *old_atual = pres_line;
    linha *old_cima = pres_line->up;

    pres_line = old_cima;
    pres_line->down = old_atual;
    old_atual->up = pres_line;

    cursor->cel = pres_line->head;
    cursor->linha--;

    int old_coluna = cursor->coluna;
    cursor->coluna = 0;

    if (pres_line->head == NULL)
        cria_celula_vazia_a_direita();
    celula *aux = cursor->cel;
    while (aux->next != NULL && old_coluna > 0)
    {
        aux = aux->next;
        cursor_frente();
        old_coluna--;
    }
}

void delete_char_a_esquerda()
{
    // verifica se o cursor e a lista estao vazios
    if (cursor == NULL || pres_line->head == NULL)
    {
        printf("Erro: cursor ou lista estao vazios\n");
        return;
    }
    celula *old_atual = cursor->cel;
    celula *anterior, *proxima;

    if (old_atual->prev != NULL)
        anterior = old_atual->prev;
    else
        anterior = NULL;
    if (old_atual->next != NULL)
        proxima = old_atual->next;
    else
        proxima = NULL;

    // linha vazia
    if (anterior == NULL && proxima == NULL)
        return;

    if (anterior != NULL)
        anterior->next = proxima;
    if (proxima != NULL)
        proxima->prev = anterior;

    // verifica se o cursor esta nas extremidades da linha
    if (old_atual == pres_line->tail)
        pres_line->tail = anterior;

    if (old_atual == pres_line->head->next)
    {
        pres_line->head->next = proxima;
        cursor_frente();
    }
    else
    {
        cursor_traz();
        free(old_atual);
    }
    // atualiza o tamanho da linha
    pres_line->tamanho--;
}

void delete_line()
{
    // verifica se o cursor e a lista estao vazios
    if (cursor == NULL || pres_line->head == NULL)
    {
        printf("Erro: cursor ou lista estao vazios\n");
        return;
    }

    linha *old_atual = pres_line;
    linha *anterior, *proxima;

    if (old_atual->up != NULL)
        anterior = old_atual->up;
    else
        anterior = NULL;

    if (old_atual->down != NULL)
        proxima = old_atual->down;
    else
        proxima = NULL;

    if (head_line == tail_line)
    {
        if (cursor->cel != pres_line->tail)
        {
            cursor->cel = pres_line->tail;
            cursor->coluna = pres_line->tamanho;
        }

        celula *aux = cursor->cel;
        while (aux->prev != NULL)
        {
            delete_char_a_esquerda();
            // full_print();
            aux = cursor->cel;
        }

        return;
    }

    if (old_atual == head_line)
    {
        head_line = proxima;
        cursor_baixo();
        pres_line->up = NULL;
        pres_line->down = proxima->down;
        pres_line = proxima;
        pres_line->head = proxima->head;
        pres_line->tail = proxima->tail;
        pres_line->tamanho = proxima->tamanho;

        (proxima->up)->down = pres_line;
    }
    else if (old_atual == tail_line)
    {
        tail_line = anterior;
        cursor_cima();
        pres_line->down = NULL;
        pres_line->up = anterior->up;
        pres_line->head = anterior->head;
        pres_line->tail = anterior->tail;
        pres_line->tamanho = anterior->tamanho;

        // (anterior->down)->up = pres_line;
    }
    else
    {
        cursor_cima();
        pres_line = anterior;
        pres_line->down = proxima;
        pres_line->up = anterior->up;
        pres_line->head = anterior->head;
        pres_line->tail = anterior->tail;
        pres_line->tamanho = anterior->tamanho;

        proxima->up = pres_line;
    }
    free(old_atual);

    numero_de_linhas--;
}

void delete_word()
{
    // verifica se o cursor e a lista estao vazios
    if (cursor == NULL || pres_line->head == NULL)
    {
        printf("Erro: cursor ou lista estao vazios\n");
        return;
    }

    while (cursor->cel->val != ' ' &&
           cursor->cel->prev != NULL)
    {
        delete_char_a_esquerda();
        pres_line->tamanho--;
    }
}

void print_line(linha *v)
{
    celula *aux = v->head;

    if (aux == NULL)
    {
        printf("\n");
        return;
    }
    if (aux->next != NULL)
        aux = aux->next;
    while (aux != NULL)
    {
        printf("%c", aux->val);
        aux = aux->next;
    }
    printf("\n");
}

void imprime_cursor()
{
    int blank = 0;
    while (numero_de_linhas >= pow(10, blank))
    {
        blank++;
        printf(" ");
    }
    // por conta da |
    printf(" ");

    for (int i = 0; i < cursor->coluna; i++)
        printf(" ");
    if (tipo_de_cursor == 0)
        printf("^\n");
    else
        printf("/\\\n");
}

void imprime_coord()
{
    printf("(%d,%d): ", cursor->linha, cursor->coluna);
}

void print_all_lines()
{
    linha *aux = head_line;
    int n = 0;

    int blank = 0;

    while (numero_de_linhas >= pow(10, blank))
        blank++;

    while (aux != NULL)
    {
        if (n == cursor->linha)
            printf(">");
        else
            printf(" ");

        printf("%*d|", blank, n);

        print_line(aux);
        if (n == cursor->linha)
            imprime_cursor();

        aux = aux->down;
        n++;
    }
}

void full_print()
{
    printf("------------------------\n");
    print_all_lines();
    printf("------------------------\n");
    imprime_coord();
}

celula *marca_posicao_atual()
{
    celula *aux = (celula *)malloc(sizeof(celula));
    if (aux == NULL)
    {
        printf("Erro ao alocar memoria para a celula\n");
        exit(1);
    }
    build_celula(aux);
    aux->next = cursor->cel->next;
    aux->prev = cursor->cel;
    aux->val = cursor->cel->val;
    return aux;
}

void volta_pra_posicao_marcada(celula *marcada, int linha, int coluna)
{
    cursor->cel = marcada;
    cursor->cel->next = marcada->next;
    cursor->cel->prev = marcada->prev;

    cursor->linha = linha;
    cursor->coluna = coluna;
}

void insert_char_a_direita(char d)
{
    // caso o cursor seja vazio
    if (cursor->cel == NULL)
    {
        celula *new = (celula *)malloc(sizeof(celula));

        if (new == NULL)
        {
            printf("Erro ao alocar memoria para a celula\n");
            exit(1);
        }

        build_celula(new);

        if ((cursor->cel)->next != NULL)
        {
            // da nome a proxima celula
            celula *b = (cursor->cel)->next;
            b->prev = new;
        }
    }

    // celula a direita do cursor
    cria_celula_vazia_a_direita();
    cursor_frente();

    (cursor->cel)->val = d;
    pres_line->tamanho++;

    while (pres_line->tail->next != NULL)
        pres_line->tail = pres_line->tail->next;
}

void insert_line_em_baixo()
{
    linha *new_line;
    new_line = (linha *)malloc(sizeof(linha));

    // verifica se a linha foi alocada
    if (new_line == NULL)
    {
        printf("Erro ao alocar memoria para a linha\n");
        exit(1);
    }

    build_linha(new_line);

    // verifica se a linha atual nao e a ultima
    if (pres_line->down != NULL)
    {
        linha *below = pres_line->down;
        pres_line->down = new_line;
        new_line->up = pres_line;
        new_line->down = below;
        below->up = new_line;
    }
    else
    {
        pres_line->down = new_line;
        new_line->up = pres_line;
    }

    celula *aux;
    aux = (celula *)malloc(sizeof(celula));
    if (aux == NULL)
    {
        printf("Erro ao alocar memoria para a celula\n");
        exit(1);
    }

    build_celula(aux);
    new_line->head = aux;
    new_line->tail = aux;

    // full_print();
    if (cursor->cel->next != NULL)
    {
        celula *prim_char = cursor->cel->next;

        pres_line->tail = cursor->cel;
        pres_line->tail->next = NULL;
        int old_tamanho = pres_line->tamanho;
        pres_line->tamanho = cursor->coluna;
        // full_print();

        aux->next = prim_char;
        prim_char->prev = aux;
        new_line->tamanho = old_tamanho - cursor->coluna;

        celula *new_tail = prim_char;
        while (new_tail->next != NULL)
            new_tail = new_tail->next;
        new_line->tail = new_tail;
        // full_print();
    }

    // move o cursor para a nova linha
    cursor->cel = new_line->head;
    cursor->linha++;
    cursor->coluna = 0;

    numero_de_linhas++;
    pres_line = new_line;

    // atualiza a ultima linha
    while (tail_line->down != NULL)
        tail_line = tail_line->down;
}

void conecta_linhas()
{
    celula *pos_atual = marca_posicao_atual();

    if (pres_line->down == NULL)
        return;

    // da nome aos bois
    linha *below_line = pres_line->down;
    linha *linha_atual = pres_line;
    linha *below_below_line = below_line->down;
    celula *below = below_line->head->next;
    celula *old_pres_tail = pres_line->tail;
    celula *old_below_tail = below_line->tail;
    int old_below_tamanho = below_line->tamanho;

    if (tail_line == below_line)
        tail_line = pres_line;
    // conecta as linhas
    pres_line->tail->next = below;
    below->prev = old_pres_tail;
    pres_line->tamanho += old_below_tamanho;
    pres_line->down = below_below_line;
    pres_line->tail = old_below_tail;
    if (below_below_line != NULL)
        below_below_line->up = linha_atual;

    numero_de_linhas--;

    free(below_line->head);
    // full_print();
}

void from_console_to_line(console *cons)
{
    for (int i = 0; i < cons->tamanho; i++)
        insert_char_a_direita(cons->letras[i]);
}

int number_no_console(console *d)
{
    if (d->letras[0] == '\0')
        return 1;

    int S = 0;
    char *end;
    char *d_letras = d->letras;

    S = (int)strtol(d_letras, &end, 10);
    if (end == d->letras)
        return 1;

    return S;
}

void move_cursor_next_palavra()
{
    while (cursor->cel != NULL &&
           cursor->cel->val != ' ')
        if (cursor->cel->next != NULL)
            cursor_frente();
}

void move_cursor_prev_palavra()
{
    while (cursor->cel != NULL &&
           cursor->cel->val != ' ')
        if (cursor->cel->prev != NULL)
            cursor_traz();
}

int copia_memoria()
{
    if (cursor->cel == NULL)
        return 0;
    ctrl_c = cursor->cel;
    return cursor->coluna;
}

void cola_memoria()
{
    if (ctrl_c == NULL)
        return;
    insert_char_a_direita(ctrl_c->val);
}

void recorta_memoria()
{
    if (cursor->cel == NULL)
        return;

    celula *atual = cursor->cel;
    int atual_coluna = cursor->coluna;

    if (ctrl_c == NULL)
        return;
    // aponta o cursor pra poder deletar
    cursor->cel = ctrl_c;
    char d = cursor->cel->val;

    delete_char_a_esquerda();

    cursor->cel = atual;

    // atualiza a coluna do cursor (cuidando pro
    // caso da exclusao estar antes ou depois do
    // cursor)
    cursor->coluna = atual_coluna;
    if (atual_coluna > ctrl_c_coluna)
        cursor->coluna--;

    insert_char_a_direita(d);
}

int tamanho_da_linha()
{
    int n = 0;
    celula *aux = head->next;
    while (aux != NULL)
    {
        n++;
        aux = aux->next;
    }
    return n;
}

char *line_to_string(linha *l)
{
    int n = l->tamanho;
    // printf("n = %d\n", n);
    char *str;
    str = (char *)malloc(n * sizeof(char));
    memset(str, '\0', sizeof(str));

    celula *aux = l->head;

    int i = 0;
    while (aux->next != NULL && i < n)
    {
        aux = aux->next;
        // printf("aux->val = %c\n", aux->val);
        str[i] = aux->val;
        i++;
    }
    // printf("str = %s\n", str);
    return str;
}

char *console_to_string(console *cons, char *str)
{
    sprintf(str, "%s", cons->letras);

    printf("str = %s\n", str);
    return str;
}

// Implementando o algoritimo de Knuth Morris Pratt
void KMP_prefixo(char *padrao, int M, int *prefixos)
{
    prefixos[0] = 0;
    int j = 0, i = 1;

    // percorre o padrao
    while (i < M)
    {
        if (padrao[i] == padrao[j])
        {
            j++;
            prefixos[i] = j;
            i++;
        }
        else
        {
            if (j != 0)
                j = prefixos[j - 1];
            else
            {
                prefixos[i] = 0;
                i++;
            }
        }
    }

    // // printa prefixos
    // for(int i=0; i<M; i++)
    //     printf("prefixos[%d] = %d\n", i, prefixos[i]);
}

int KMP_busca(console *cons)
{
    // tamanho do padrao
    int M = cons->tamanho;
    // tamanho do texto (dark necessities: add one)
    int N = pres_line->tamanho - cursor->coluna +1;
    char padrao[M];

    for (int i = 0; i < M; i++)
        padrao[i] = cons->letras[i];

    int prefixos[M];

    // calcula o prefixo e o sufixo do padrao
    KMP_prefixo(padrao, M, prefixos);

    int j = 0, i = 0;
    // "texto[i]"
    celula *texto_i = cursor->cel;

    while (i < N)
    {
        // printf("padrao[%d] = %c, texto_%d->val = %c\n", j, padrao[j], i, texto_i->val);

        if (padrao[j] == texto_i->val)
        {
            j++;
            i++;
            if (texto_i->next != NULL)
                texto_i = texto_i->next;
            else
                break;
        }

        if (j == M)
            return i - j;
        else if (i < N && padrao[j] != texto_i->val)
        {
            // printf("i=%d", i);
            if (j != 0)
                j = prefixos[j - 1];
            else
            {
                i++;
                if (texto_i->next != NULL)
                    texto_i = texto_i->next;
                else
                    break;
            }
        }
    }

    if(j==M)
        return i-j;

    return -1;
}

bool mecanismo_de_busca(console *cons)
{
    int ind_kmp = KMP_busca(cons);
    int n = cons->tamanho;

    if (ind_kmp == -1)
        return false;

    // anda ate o indice encontrado pelo metodo
    while (ind_kmp > 0)
    {
        cursor_frente();
        ind_kmp--;
    }
    full_print();
    return true;
}

void performa_busca(console *cons)
{
    // primeiro caso comeca a busca a partir do inicio da linha
    point_to_master_head();
    cursor_frente();

    if (cons == NULL)
        return;

    bool search_next = true;

    while (search_next == true)
    {
        search_next = mecanismo_de_busca(cons);

        if (search_next == true)
        {
            // caso encontre, pergunta se quer continuar a busca
            printf("Deseja continuar a busca? (s/n) ");
            char c = getchar();
            // limpa o buffer
            fflush(stdin);
            while (c != 's' && c != 'n')
                c = getchar();
            if (c == 'n')
                break;
            else // anda uma posicao a frente
                cursor_frente();
        }

        if (search_next == false && cursor->cel->next != NULL)
        {
            cursor_frente();
            search_next = true;
        }
        // else

        if (search_next == false && pres_line->down != NULL)
        {
            cursor_baixo();
            // aponta pro inicio da linha
            cursor->cel = pres_line->head->next;
            cursor->coluna = 1;
            search_next = true;
        }
    }
}

void escreve_arquivo(console *cons_input)
{
    // variaveis necessarias pra abrir um arquivo
    char *file_name;
    FILE *arq;

    if (head_line == NULL)
    {
        printf("Nao ha nada para salvar\n");
        return;
    }
    file_name = (char *)malloc((cons_input->tamanho) * sizeof(char));

    // printf("cons_input->letras = %s\n", cons_input->letras);

    snprintf(file_name, cons_input->tamanho, "%s", cons_input->letras);

    file_name[cons_input->tamanho] = '\0';

    // printf("file_name = %s\n", file_name);

    arq = fopen(file_name, "w");
    if (arq == NULL)
    {
        printf("Erro ao criar o arquivo\n");
        return;
    }

    char *aux;
    linha *aux_line = head_line;

    for (int i = 0; i < numero_de_linhas; i++)
    {
        // checa se a linha atual eh a ultima
        if (aux_line == NULL)
            break;
        // printf("Escrevendo linha %d\n", i);
        aux = line_to_string(aux_line);
        fprintf(arq, "%s\n", aux);
        aux_line = aux_line->down;
    }
    printf("Arquivo %s salvo com sucesso!\n", file_name);
    fclose(arq);
    free(file_name);
}

void lida_com_texto_ja_escrito()
{
    char s = 'a';
    console *nome_do_arquivo;
    nome_do_arquivo = (console *)malloc(sizeof(console));
    build_console(nome_do_arquivo);

    printf("Deseja salvar o arquivo atual antes de sair?\n");
    while (s != 's' && s != 'n')
    {
        printf("[s] salvar [n] sair sem salvar: ");
        fflush(stdin); // limpa o buffer de entrada
        scanf("%c", &s);
        int d;
        while ((d = getchar()) != '\n' && d != EOF)
            ;
    }

    if (s == 's')
    {
        printf("\nNome do arquivo: ");
        preenche_console(nome_do_arquivo);

        printf("\nSalvando arquivo...\n");
        escreve_arquivo(nome_do_arquivo);

        printf("\nArquivo salvo com sucesso!\n");
    }
    free(nome_do_arquivo);

    point_to_master_tail();

    // full_print();
    // limpa a tela
    while (numero_de_linhas > 2)
        delete_line();
    delete_line();
}

void le_um_documento(console *cons)
{
    char *file_name;
    FILE *arq;

    if (head_line == NULL)
    {
        printf("Não há nada para salvar\n");
        return;
    }
    file_name = (char *)malloc((cons->tamanho) * sizeof(char));

    // printf("cons_input->letras = %s\n", cons_input->letras);

    snprintf(file_name, cons->tamanho, "%s", cons->letras);

    file_name[cons->tamanho] = '\0';

    // printf("file_name = %s\n", file_name);

    arq = fopen(file_name, "r");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    char ch;
    point_to_master_head();

    do
    {
        ch = fgetc(arq);

        if (ch == '\n')
            insert_line_em_baixo();
        else if (ch != EOF)
            insert_char_a_direita(ch);
    } while (ch != EOF);

    fclose(arq);
    free(file_name);

    delete_line();

    // move cursor pro final do arquivo
    point_to_master_tail();
}

void move_cursor_to_line(console *cons)
{
    int x, abs_dist; // linha to go
    char d = cons->letras[0];

    if (d == '0')
        point_to_master_head();
    else if (isdigit(d) == 1)
    {
        x = number_no_console(cons);

        // distancia entre atual e x
        abs_dist = abs(x - cursor->linha);

        if (abs_dist == 0)
            return;

        while (abs_dist > 0)
        {
            if (x > cursor->linha)
                cursor_baixo();
            else if (x < cursor->linha)
                cursor_cima();
            abs_dist--;
        }
    }
    else if (d == 'F')
    {
        // vai pra ultima linha
        pres_line = tail_line;
        cursor->cel = pres_line->head;
        cursor->coluna = 0;
        cursor->linha = numero_de_linhas - 1;
    }
}

int parse(console *cons)
{
    // pega o primeiro caractere da string
    char c = cons->letras[0];
    delete_char_console(cons, 0);

    // numero de iteracoes de uma dada funcao
    int iteradas = 1;
    // uma vez que o numero de iteradas da funcao ja esta armazenado,
    // removemos ele do console
    if (!check_vec(c, lista_f_come_string,
                   NUM_FUNCOES_COMEM_STRING))
    {
        iteradas = number_no_console(cons);
        while (isdigit(cons->letras[0]))
            delete_char_console(cons, 0);
        if (iteradas == 0)
            iteradas = 1;
    }

    // realiza a funcao de acordo com o caractere
    if (isdigit(c) == false)
    {
        switch (c)
        {
        case 'G':
            tipo_de_cursor = (tipo_de_cursor + 1) % 2;
            break;
        case 'H': // vai pra linha anterior
            while (iteradas > 0)
            {
                if (pres_line->up != NULL)
                    cursor_cima();
                iteradas--;
            }
            break;
        case ':': // move o cursor para o inicio da linha x
            move_cursor_to_line(cons);
            break;
        case 'J':
            while (iteradas > 0)
            {
                if (pres_line->down != NULL)
                    cursor_baixo();
                iteradas--;
            }
            break;
        case 'N':
            while (iteradas > 0)
            {
                insert_line_em_baixo();
                iteradas--;
            }
            break;
        case 'U':
            while (iteradas > 0)
            {
                conecta_linhas();
                iteradas--;
            }
            break;
        case 'I':
            if (cons != NULL)
                from_console_to_line(cons);
            break;
        case 'E':
            escreve_arquivo(cons);
            break;
        case 'A': // abre um arquivo
            if (head_line->head->next != NULL)
                ;
            lida_com_texto_ja_escrito();
            le_um_documento(cons);
            break;
        case 'D':
            if (cons->letras[0] == 'L')
            {
                delete_char_console(cons, 0);
                if (!check_vec(c, lista_f_come_string,
                               NUM_FUNCOES_COMEM_STRING))
                {
                    iteradas = number_no_console(cons);
                    while (isdigit(cons->letras[0]))
                        delete_char_console(cons, 0);
                    if (iteradas == 0)
                        iteradas = 1;
                }

                while (iteradas > 0)
                {
                    delete_line();
                    iteradas--;
                }
                break;
            }

            if (cons->letras[0] == 'W')
            {
                delete_char_console(cons, 0);
                if (!check_vec(c, lista_f_come_string,
                               NUM_FUNCOES_COMEM_STRING))
                {
                    iteradas = number_no_console(cons);
                    while (isdigit(cons->letras[0]))
                        delete_char_console(cons, 0);
                    if (iteradas == 0)
                        iteradas = 1;
                }

                while (iteradas > 0)
                {
                    delete_word();
                    iteradas--;
                }
                break;
            }
            while (iteradas > 0)
            {
                delete_char_a_esquerda();
                iteradas--;
            }
            break;
        case 'B':
            performa_busca(cons);
            break;
        case 'F': // move cursor pra cursor_frente
            while (iteradas > 0)
            {
                if ((cursor->cel)->next != NULL)
                    cursor_frente();
                iteradas--;
            }
            break;
        case 'T': // move cursor pra cursor_traz
            while (iteradas > 0)
            {
                if ((cursor->cel)->prev != NULL)
                    cursor_traz();
                iteradas--;
            }
            break;
        case 'O':
            if (cursor->cel != pres_line->head)
            {
                cursor->cel = pres_line->head;
                cursor->coluna = 0;
            }
            break;
        case '$':
            if (cursor->cel != pres_line->tail)
            {
                cursor->cel = pres_line->tail;
                cursor->coluna = pres_line->tamanho;
            }
            break;
        case 'P':
            while (iteradas > 0)
            {
                move_cursor_next_palavra();
                iteradas--;
            }
            break;
        case 'Q':
            while (iteradas > 0)
            {
                move_cursor_prev_palavra();
                iteradas--;
            }
            break;
        case 'C':
            ctrl_c_coluna = copia_memoria();
            break;
        case 'V':
            while (iteradas > 0)
            {
                cola_memoria();
                iteradas--;
            }
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

    while (pres_line->tail->next != NULL)
        pres_line->tail = pres_line->tail->next;

    // passo recursivo: casos expurios
    if (cons->tamanho > 1 &&
        !check_vec(c, lista_f_come_string, NUM_FUNCOES_COMEM_STRING))
    {
        parse(cons);
        return 0;
    }
    full_print();

    return 0;
}

int main()
{
    // possibilita usar caracteres
    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(65001);
    freopen("CON", "w", stdout);

    // cursor = (x,y) da coordenada cursor
    cursor = (ponto *)malloc(sizeof(ponto));
    cursor->linha = 0;
    cursor->coluna = 0;

    // linhas principais
    head_line = (linha *)malloc(sizeof(linha));
    tail_line = (linha *)malloc(sizeof(linha));
    pres_line = (linha *)malloc(sizeof(linha));
    build_linha(pres_line);

    head_line = pres_line;
    tail_line = pres_line;

    console *v;
    v = (console *)malloc(sizeof(console));
    build_console(v);

    head = (celula *)malloc(sizeof(celula));
    tail = (celula *)malloc(sizeof(celula));

    celula *cel;
    cel = (celula *)malloc(sizeof(celula));
    build_celula(cel);

    head = cel;
    tail = cel;

    pres_line->head = head;
    pres_line->tail = tail;

    cursor->cel = cel;

    int retval;
    full_print();
    do
    {
        preenche_console(v);
        retval = parse(v);
        reseta_console(v);
    } while (retval != 2);

    return 0;
}