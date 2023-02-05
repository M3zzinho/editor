#include "editor.h"

// lista de funcoes implementadas
extern char lista_f[NUM_FUNCOES];

extern char lista_f_come_string[NUM_FUNCOES_COMEM_STRING];

// Variaveis globais
ponto *cursor;
celula *head, *tail;
linha *pres_line, *head_line, *tail_line;

int numero_de_linhas = 1;
int tipo_de_cursor = 0;
ponto* ctrl_c;

void imprime_coord()
{
    printf("(%d,%d): ", cursor->linha, cursor->coluna);
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

void from_console_to_line(console *cons)
{
    for (int i = 0; i < cons->tamanho; i++)
        insert_char_a_direita(cons->letras[i]);
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
    cursor_tras();
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
        case 'T': // move cursor pra cursor_tras
            while (iteradas > 0)
            {
                if ((cursor->cel)->prev != NULL)
                    cursor_tras();
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
    setlocale(LC_ALL, "pt_BR.UTF-8");
#ifdef _WIN32
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
