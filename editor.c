#include "editor.h"

// lista de funcoes implementadas
extern char lista_f[NUM_FUNCOES];

extern char lista_f_come_string[NUM_FUNCOES_COMEM_STRING];

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

// Variaveis globais
ponto *cursor;
celula *head, *tail;
linha *pres_line, *head_line, *tail_line;

int numero_de_linhas = 1;
int tipo_de_cursor = 0;
ponto* ctrl_c;

// Funcoes do console
/*****************************/
void build_console(console *v)
{
    v->tamanho = 0;
    v->letras = (char *)malloc(cap_local * sizeof(char));

    if (v->letras == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria para o console\n");
        exit(1);
    }
    v->letras[cap_local - 1] = '\0';
}

void build_ponto(ponto *new_ponto)
{
    new_ponto->linha = 0;
    new_ponto->coluna = 0;
    new_ponto->cel = NULL;
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
void build_celula(celula *new_cel)
{
    new_cel->val = '\0';
    new_cel->next = NULL;
    new_cel->prev = NULL;
}

void build_linha(linha *new_line)
{
    new_line->tamanho = 0;
    new_line->head = NULL;
    new_line->tail = NULL;
    new_line->up = NULL;
    new_line->down = NULL;
}

void cria_celula_vazia_a_direita()
{
    celula *new_cel = (celula *)malloc(sizeof(celula));
    if (new_cel == NULL)
    {
        fprintf(stderr, "Erro ao alocar memoria para a celula\n");
        exit(1);
    }

    build_celula(new_cel);

    if ((cursor->cel)->next != NULL)
    {
        // da nome a proxima celula
        celula *right = (cursor->cel)->next;
        (cursor->cel)->next = new_cel;
        new_cel->prev = cursor->cel;
        new_cel->next = right;
        right->prev = new_cel;
    }
    else
    {
        (cursor->cel)->next = new_cel;
        new_cel->prev = cursor->cel;
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
    // while (tail_line->down != NULL)
    //     tail_line = tail_line->down;
    // while (tail_line->tail->next != NULL)
    //     tail_line->tail = tail_line->tail->next;

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
        fprintf(stderr, "Erro: cursor vazio\n");
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
        fprintf(stderr, "Erro: cursor vazio\n");
        return;
    }
    if (pres_line->head == NULL ||
        (cursor->cel)->prev == NULL)
        return;

    cursor->cel = (cursor->cel)->prev;
    cursor->coluna--;
}

void cursor_cima()
{
    if (cursor->cel == NULL)
    {
        fprintf(stderr, "Erro: cursor vazio\n");
        return;
    }
    if (pres_line->up == NULL)
        return;

    linha *old_cima = pres_line->up;

    pres_line = old_cima;

    int dist_head, dist_tail;
    dist_head = abs(cursor->coluna);
    dist_tail = abs(old_cima->tamanho - cursor->coluna);

    // movimenta o cursor para a extremidade mais próxima
    if (dist_head < dist_tail)
    {
        cursor->cel = old_cima->head;
        cursor->coluna = 0;
    }
    else
    {
        cursor->cel = old_cima->tail;
        cursor->coluna = old_cima->tamanho;
    }
    cursor->linha--;

    if (pres_line->head == NULL)
        cria_celula_vazia_a_direita();

    int passos = min(dist_head, dist_tail);

    // movimenta o cursor para a coluna desejada
    celula *aux = cursor->cel;
    while (passos > 0)
    {
        if (dist_head < dist_tail)
        {
            if(aux->next == NULL)
                break;
            aux = aux->next;
            cursor_frente();
        }
        else
        {
            if(aux->prev == NULL)
                break;
            aux = aux->prev;
            cursor_traz();
        }

        passos--;
        // full_print();
    }
}

void cursor_baixo(){
    if (cursor->cel == NULL)
    {
        fprintf(stderr, "Erro: cursor vazio\n");
        return;
    }
    if (pres_line->down == NULL)
        return;

    linha *old_baixo = pres_line->down;

    pres_line = old_baixo;

    int dist_head, dist_tail;
    dist_head = abs(cursor->coluna);
    dist_tail = abs(old_baixo->tamanho - cursor->coluna);

    // movimenta o cursor para a extremidade mais próxima
    if (dist_head < dist_tail)
    {
        cursor->cel = old_baixo->head;
        cursor->coluna = 0;
    }
    else
    {
        cursor->cel = old_baixo->tail;
        cursor->coluna = old_baixo->tamanho;
    }
    cursor->linha++;

    if (pres_line->head == NULL)
        cria_celula_vazia_a_direita();

    int passos = min(dist_head, dist_tail);

    // movimenta o cursor para a coluna desejada
    celula *aux = cursor->cel;
    while (passos > 0)
    {
        if (dist_head < dist_tail)
        {
            if(aux->next == NULL)
                break;
            aux = aux->next;
            cursor_frente();
        }
        else
        {
            if(aux->prev == NULL)
                break;
            aux = aux->prev;
            cursor_traz();
        }

        passos--;
        // full_print();
    }
}

void delete_char()
{
    // verifica se o cursor e a lista estao vazios
    if (cursor == NULL || pres_line->head == NULL)
    {
        fprintf(stderr, "Erro: cursor ou lista estao vazios\n");
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
        fprintf(stderr, "Erro: cursor ou lista estao vazios\n");
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

    if(numero_de_linhas != 1)
        numero_de_linhas--;

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
            delete_char();
            // full_print();
            aux = cursor->cel;
        }

        point_to_master_head();
        return;
    }

    if (old_atual == head_line)
    {
        head_line = proxima;
        head_line->up = NULL;
        point_to_master_head();
    }
    else if (old_atual == tail_line)
    {
        tail_line = anterior;
        tail_line->down = NULL;
        point_to_master_tail();
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
}

void delete_word()
{
    // verifica se o cursor e a lista estao vazios
    if (cursor == NULL || pres_line->head == NULL)
    {
        fprintf(stderr, "Erro: cursor ou lista estao vazios\n");
        return;
    }

    while (cursor->cel->val != ' ' &&
           cursor->cel->prev != NULL)
    {
        delete_char();
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

int power(int base, int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++)
        result *= base;
    return result;
}

void imprime_cursor()
{
    int blank = 0;
    while (numero_de_linhas >= power(10, blank))
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

    while (numero_de_linhas >= power(10, blank))
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
        celula *new_cel = (celula *)malloc(sizeof(celula));

        if (new_cel == NULL)
        {
            printf("Erro ao alocar memoria para a celula\n");
            exit(1);
        }

        build_celula(new_cel);

        if ((cursor->cel)->next != NULL)
        {
            // da nome a proxima celula
            celula *b = (cursor->cel)->next;
            b->prev = new_cel;
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

void copia_memoria()
{
    build_ponto(ctrl_c);

    if (cursor->cel == NULL)
        return;

    int lin = cursor->linha;
    int col = cursor->coluna;
    char c = cursor->cel->val;

    ctrl_c->cel = cursor->cel;
    ctrl_c->linha = lin;
    ctrl_c->coluna = col;
    ctrl_c->cel->val = c;
}

void cola_memoria()
{
    if (ctrl_c == NULL)
        return;

    if(ctrl_c->cel == NULL)
        return;

    insert_char_a_direita(ctrl_c->cel->val);
    // build_ponto(ctrl_c);
}

void recorta_memoria()
{
    if (ctrl_c->cel == NULL)
        return;

    if (cursor->cel == NULL)
        return;

    ponto *atual = cursor;

    if (ctrl_c == NULL)
        return;

    // aponta o cursor pra poder deletar
    cursor = ctrl_c;
    char d = cursor->cel->val;

    delete_char();

    cursor = atual;

    // atualiza a coluna do cursor (cuidando pro
    // caso da exclusao estar antes ou depois do
    // cursor)
    if (atual->coluna > ctrl_c->coluna)
        cursor->coluna--;

    insert_char_a_direita(d);

    // build_ponto(ctrl_c);
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
    char *str = (char *) malloc(n * sizeof(char));
    memset(str, '\0', n * sizeof(char));

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
    int N = pres_line->tamanho - cursor->coluna + 1;
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

    if (j == M)
        return i - j;

    return -1;
}

bool mecanismo_de_busca(console *cons)
{
    int ind_kmp = KMP_busca(cons);

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

void substitui(console *old, console *new_cons)
{
    cursor_traz();
    // apaga o antigo
    for (int i = 0; i < old->tamanho; i++)
    {
        cursor_frente();
        delete_char();
    }
    // insere o novo
    for (int i = 0; i < new_cons->tamanho; i++)
        insert_char_a_direita(new_cons->letras[i]);
}

void performa_busca(console *cons, bool substituir)
{
    // primeiro caso comeca a busca a partir do inicio da linha
    point_to_master_head();
    cursor_frente();

    if (cons == NULL)
        return;

    bool search_next = true;

    bool old_substituir = substituir;

    while (search_next == true)
    {
        search_next = mecanismo_de_busca(cons);

        if (search_next == true)
        {
            // caso encontre, pergunta se quer substituir
            if (substituir == true)
            {
                printf("Deseja substituir? [s/n] ");
                char c = getchar();
                // limpa o buffer
                fflush(stdin);
                while (c != 's' && c != 'n')
                    c = getchar();
                if (c == 's')
                    substituir = true;
                else
                    substituir = false;
            }

            if (substituir == true)
            {
                console *new_cons = (console *)malloc(sizeof(console));
                build_console(new_cons);

                printf("Substituir por: ");
                preenche_console(new_cons);

                substitui(cons, new_cons);
                free(new_cons);

                printf("\n");
                full_print();
            }

            // caso encontre, pergunta se quer continuar a busca
            printf("Deseja continuar a busca? [s/n] ");
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

        if (search_next == false && pres_line->down != NULL)
        {
            cursor_baixo();
            while (pres_line->head->next == NULL && pres_line->down != NULL)
                cursor_baixo();
            // aponta pro inicio da linha
            cursor->cel = pres_line->head->next;
            cursor->coluna = 1;
            search_next = true;
        }

        substituir = old_substituir;
    }
}

void escreve_arquivo(console *cons_input)
{
    // variaveis necessarias pra abrir um arquivo
    FILE *arq;

    if (head_line == NULL)
    {
        printf("Nao ha nada para salvar\n");
        return;
    }

    arq = fopen(cons_input->letras, "w");
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
    printf("Arquivo %s salvo com sucesso!\n", cons_input->letras);
    fclose(arq);
}

void lida_com_texto_ja_escrito()
{
    char s;
    int ret = 0;
    console *nome_do_arquivo;
    nome_do_arquivo = (console *)malloc(sizeof(console));
    build_console(nome_do_arquivo);

    printf("Deseja salvar o arquivo atual antes de sair? [s/n] ");
    // rode ate que o usuario coloque um input valido
    while (ret != 1 || (s != 's' && s != 'n'))
    {
        ret = scanf("%c", &s);
        // limpa o buffer
        fflush(stdin);
        if (ret != 1)
            printf("Entrada invalida, tente novamente: ");
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
    FILE *arq;

    if (head_line == NULL)
    {
        printf("Não há nada para salvar\n");
        return;
    }

    // printf("cons_input->letras = %s\n", cons_input->letras);

    printf("file_name = %s\n", cons->letras);

    arq = fopen(cons->letras, "r");
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

    delete_line();

    // move cursor pro final do arquivo
    point_to_master_tail();
}

void move_cursor_to_line(console *cons)
{
    int x, abs_dist, head_dist, tail_dist;
    char d = cons->letras[0];

    if (d == '0')
        point_to_master_head();
    else if (isdigit(d) == 1)
    {
        x = number_no_console(cons);

        // distancias
        abs_dist = abs(x - cursor->linha);
        head_dist = abs(x);
        tail_dist = abs(numero_de_linhas - x);

        // ve qual referencia eh mais proxima
        int dist = min(min(head_dist, tail_dist), abs_dist);

        if (dist == head_dist)
            point_to_master_head();
        else if (dist == tail_dist)
            point_to_master_tail();

        if (dist == 0)
            return;

        while (dist> 0)
        {
            if (x > cursor->linha)
                cursor_baixo();
            else if (x < cursor->linha)
                cursor_cima();
            dist--;
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

    // deleta os numeros do console
    while (isdigit(cons->letras[0]))
        delete_char_console(cons, 0);
}

int parse(console *cons)
{
    // pega o primeiro caractere da string
    char c = cons->letras[0];
    delete_char_console(cons, 0);
    printf("cons->letras = %s", cons->letras);

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
                delete_char();
                iteradas--;
            }
            break;
        case 'B':
            performa_busca(cons, false);
            break;
        case 'S':
            performa_busca(cons, true);
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
            copia_memoria();
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
#ifdef _WIN32
    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(65001);
    freopen("CON", "w", stdout);
#endif

    cursor = (ponto *)malloc(sizeof(ponto));
    build_ponto(cursor);

    ctrl_c = (ponto *)malloc(sizeof(ponto));
    build_ponto(ctrl_c);

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
