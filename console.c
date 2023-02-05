#include "console.h"

extern ponto *cursor;
extern linha *pres_line, *head_line, *tail_line;
extern int numero_de_linhas, tipo_de_cursor;

celula *head;

void build_linha(linha *new_line)
{
    new_line->tamanho = 0;
    new_line->head = NULL;
    new_line->tail = NULL;
    new_line->up = NULL;
    new_line->down = NULL;
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

void build_celula(celula *new_cel)
{
    new_cel->val = '\0';
    new_cel->next = NULL;
    new_cel->prev = NULL;
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

void cursor_tras()
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
            cursor_tras();
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
            cursor_tras();
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
        cursor_tras();
        free(old_atual);
    }
    // atualiza o tamanho da linha
    pres_line->tamanho--;
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
            cursor_tras();
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
