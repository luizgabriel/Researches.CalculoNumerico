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
#define MAX_FRAC_DIGITS 20

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
    int size = MAX_INT_DIGITS + MAX_FRAC_DIGITS + 2;

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
 * Libera uma matriz M de L linhas
 */
void delete_matrix(double** m, int l)
{
    int j;
    for (j = 0; j < l; j++)
        free(m[j]);

    free(m);
}

/**
 * Aloca uma matriz de L linhas por C colunas
 */
double **alloc_matrix(int l, int c)
{
    double **m;
    int i;
    m = malloc(sizeof(m) * l);

    if (m != NULL) {
        for (i = 0; i < l; i++) {
            m[i] = malloc(sizeof(double) * c);
            if (m[i] == NULL) {
                delete_matrix(m, l);
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
 * Limpa a saída do console
 */
void clear_console()
{
    fflush(stdout);
    printf("\033c");
}

/**
 * Imprime o menu
 */
void print_menu()
{
    int i;

    clear_console();

    printf("\n"); for (i=0; i<40; i++) printf("-");
    printf("\nCALCULO NUMERICO");
    printf("\n"); for (i=0; i<40; i++) printf("-");
    printf("\nC - Conversão");
    printf("\nS - Sistema Linear");
    printf("\nE - Equação Algébrica");
    printf("\nF - Finalizar");
    printf("\n"); for (i=0; i<40; i++) printf("-");
}

/**
 * Ler opção do menu 
 */
char read_menu_option() 
{
    char option;

    print_menu();
    printf("\n Escolha uma opção: ");
    scanf("%c", &option);

    return toupper(option);
}

/**
 * Ação de CONVERTER números para as bases HEXA, OCTAL e BINARIO 
 */
void convert_option()
{
    double number;
    char output[MAX_INT_DIGITS + MAX_FRAC_DIGITS + 2];

    clear_console();
    printf("\nInforme um número decimal: ");
    scanf("%lf", &number);

    format_number(number, BINARIO, output);
    printf("\nBINARIO: %s", output);

    format_number(number, OCTAL, output);
    printf("\nOCTAL:   %s", output);

    format_number(number, HEXA, output);
    printf("\nHEXA:    %s", output);
}

/**
 * Lê um arquivo dado, popula a matriz M de N variaveis
 * Retorno:
 *  0 - Não foi possível ler o arquivo
 *  1 - Sucesso!
 */
int read_matrix_file(char* file_name, int* size, double*** m)
{
    FILE *fp;
    int n, j, k;
    double fl;

    fp = fopen(file_name, "r");
    if (fp) {
        fscanf(fp, "%d", &n);

        *size = n;
        *m = alloc_matrix(n, n + 1);

        for (j = 0; j < n; j++) {
            for (k = 0; k < n + 1; k++) {
                //Se a matriz tiver incompleta
                if (feof(fp)) {
                    fclose(fp);
                    delete_matrix(*m, n);
                    *m = NULL;
                    *size = 0;

                    return -3;
                }

                fscanf(fp, "%lf", &fl);
                (*m)[j][k] = fl;
            }
        }

        fclose(fp);
        return 1;
    } else {
        return -1;
    }
}

/**
 * Ação de ler um arquivo dado o nome resolver a matriz usando metodo de jordan e exibir a matriz diagonal
 */
void linear_system_option()
{
    char file_name[255];
    double** m;
    int n, file_status;

    printf("\nInforme o nome de um arquivo para leitura: ");
    scanf("%s", file_name);

    file_status = read_matrix_file(file_name, &n, &m);
    if (file_status == -1) {
        printf("\n Arquivo não encontrado (%s)", file_name);
    } else if (file_status == -3) {
        printf("\n Arquivo formatado incorretamente. Forneça todos os dados. (%s)", file_name);
    } else {
        printf("\nN = %d\n", n);
        print_matrix(m, n, n + 1);
        //TODO: Aplicar metodo de JORDAN (usar funcao sl_jordan)
        //TODO: Exibir matriz diagonal (usar funcao print_matrix)
        //TODO: Exibir solução do problema (usar funcao print_solution)

        delete_matrix(m, n);
    }
}

/**
 * Ler equaçao algebrica e aplicar teoremas de lagrange, bolzano e bissessão
 */
void equation_option()
{
    //TODO
}

int main()
{
    char opt;
    while (opt = read_menu_option(), opt != FINISH_OPT) {
        if (opt == CONVERT_OPT)
            convert_option();
        else if (opt == EQUATION_OPT)
            equation_option();
        else if (opt == LINEAR_SYSTEM_OPT)
            linear_system_option();
        else {
            printf("\n OPCAO INVÁLIDA");
        }
        
        printf("\n\nPrecione ENTER para voltar para o menu...\n");
        while ((opt = getchar()) != '\n' && opt != EOF);
        getchar();
    }

    return 0;
}