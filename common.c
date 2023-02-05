#include "common.h"

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

int power(int base, int exp)
{
    int result = 1;
    for (int i = 0; i < exp; i++)
        result *= base;
    return result;
}
