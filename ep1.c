#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SL_RESULT_INCOMPATIBLE 2
#define SL_RESULT_UNDEFINED 1
#define SL_RESULT_DEFINED 0
#define SL_EPSILON 0.000001

#define SPACE_CHAR 32
#define SEPARATOR_CHAR '.'
#define MAX_INT_DIGITS 20
#define MAX_FLOAT_DIGITS 20

#define HEXA 16
#define OCTAL 8
#define BINARIO 2

#define CONVERT_OPT 'C'
#define LINEAR_SYSTEM_OPT 'S'
#define EQUATION_OPT 'E'
#define FINISH_OPT 'F'

/**
 * Converte número para caractere especifico da base
 */
char get_num(int num)
{
    if (num < 10)
        return 48 + num;
    else
        return 55 + num;
}

/**
 * Formatar Número para qualquer base
 * Entrada:
 * - num    => número em decimal a ser convertido
 * - base   => a base de destino
 * - output => um ponteiro para uma string de saída
 */
void format_number(double num, int base, char *output)
{
    int in = (int)num;
    double fl = num - in;
    int k;
    int size = MAX_INT_DIGITS + MAX_FLOAT_DIGITS + 2;

    output[size - 1] = 0;

    for (k = 0; k < size - 1; k++)
        output[k] = SPACE_CHAR;

    k = MAX_INT_DIGITS - 1;
    while (in >= base && k >= 0)
    {
        output[k] = get_num(in % base);
        in = in / base;
        k--;
    }
    output[k] = get_num(in);

    if (fl > 0)
    {
        output[MAX_INT_DIGITS] = SEPARATOR_CHAR;

        k = MAX_INT_DIGITS + 1;
        while (fl > 0 && k < size - 1)
        {
            fl = fl * base;
            output[k] = get_num((int)fl);

            fl = fl - ((int)fl);
            k++;
        }
    }
}

/**
 * Imprime uma matriz M de L linhas e C colunas
 */
void print_matrix(double** m, int l, int c)
{
    int i, j;
    for (i = 0; i < l; i++) {
        for (j = 0; j < c; j++) {
            printf("%10.3lf ", m[i][j]);
        }
        printf("\n");
    }
}

/**
 * Imprime um vetor de solução V
 */
void print_solution(double *v, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("x[%d] = %10.3lf ", i + 1, v[i]);
    }
}

/**
 * Aloca uma matriz de L linhas por C colunas
 */
double **alloc_matrix(int l, int c)
{
    double **m;
    int i, j;
    m = malloc(sizeof(m) * l);

    if (m != NULL) {
        for (i = 0; i < l; i++) {
            m[i] = malloc(sizeof(double) * c);
            if (m[i] == NULL) {
                for (j = 0; j < i; j++) { free(m[j]); }

                free(m);
                return NULL;
            }
        }
    }

    return m;
}

/**
 * Aloca um vetor de N elementos
 */
double *alloc_vector(int n)
{
    double *x = malloc(sizeof(double) * n);
    return x;
}

/**
 * Aplica método da substituição simples para resolver um sistema linear diagonal (triangular superior e inferior) 
 */
int sl_simple(double **m, int n, double *x)
{
    int i, type = SL_RESULT_DEFINED;

    for (i = 0; i < n; i++) {
        if (m[i][i] == 0) {
            type = (m[i][n] == 0)? SL_RESULT_UNDEFINED : SL_RESULT_INCOMPATIBLE;
            x[i] = 0;
        } else {
            x[i] = m[i][n] / m[i][i];
        }
    }

    return type;
}

/**
 * Aplica o método de Gauss e escalona a matriz M dada em uma matriz Triangular Superior equivalente
 */
void sl_gauss(double **m, int n)
{
    int i, j, k;
    double mult, *aux;

    for (i = 0; i < n - 1; i++) {
        if (m[i][i] == 0) { //pivo nulo
            j = i + 1;
            while (j < n && m[j][i] == 0) { j++; }
            if (j < n) { 
                //trocando as linhas i e j
                aux = m[i];
                m[i] = m[j];
                m[j] = aux;
            }
        }

        if (m[i][i] != 0) {
            for (j = i + 1; j < n; j++) {
                mult = -m[j][i] / m[i][i];
                m[j][i] = 0;
                
                //pivotando a linha j
                for (k = i + 1; k <= n; k++) {
                    m[j][k] += mult * m[i][k];
                }
            }
        }
    }
}

/**
 * Imprime o menu
 */
void print_menu()
{
    int i;

    fflush(stdout);
    printf("\n");
    for (i=0; i<40; i++) printf("-");
    printf("\nCALCULO NUMERICO");
    for (i=0; i<40; i++) printf("-");
    printf("\nC - Conversão");
    printf("\nS - Sistema Linear");
    printf("\nE - Equação Algébrica");
    printf("\nF - Finalizar");
    for (i=0; i<40; i++) printf("-");
}

/**
 * Ler opção do menu 
 */
char read_menu_option() 
{
    print_menu();
    printf("\n Escolha uma opção: ");

    return toupper(getchar());
}

/**
 * Ação de CONVERTER números para as bases HEXA, OCTAL e BINARIO 
 */
void convert_action()
{
    //TODO
}

/**
 * Ação de ler um arquivo dado o nome resolver a matriz usando metodo de jordan e exibir a matriz diagonal
 */
void linear_system_action()
{
    //TODO
}

/**
 * Ler equaçao algebrica e aplicar teoremas de lagrange, bolzano e bissessão
 */
void equation_action()
{
    //TODO
}

int main()
{
    char opt;
    while (opt = read_menu_option(), opt != FINISH_OPT) {
        if (opt == CONVERT_OPT)
            convert_action();
        else if (opt == EQUATION_OPT)
            equation_action();
        else if (opt == LINEAR_SYSTEM_OPT)
            linear_system_action();
        else {
            printf("\n Opção inválida");
        }
        
        printf("Precione qualquer tecla para voltar para o menu...");
        getchar();
    }

    return 0;
}